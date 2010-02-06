/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#include "icetray/I3Frame.h"

#include "hdf-writer/internals/I3TableWriter.h"
#include "hdf-writer/internals/I3TableService.h"
#include "hdf-writer/internals/I3ConverterFactory.h"

/******************************************************************************/

I3TableWriter::I3TableWriter(I3TableServicePtr service) {
    service_ = service;
    ticConverter_ = BuildConverter("I3IndexColumnsGenerator");
}

/******************************************************************************/

I3TableWriter::~I3TableWriter() {};

/******************************************************************************/

// register one specific object. if type and converter are empty the writer 
// should figure out appropriate values
void I3TableWriter::AddObject(std::string name, std::string tableName, 
                              std::string type, std::string converterName) {

    // figure out converter.
    I3ConverterPtr converter;
    if (converterName == "") { 
        // no converter specified. use type to figure out which one to use
        std::map<std::string,I3ConverterPtr>::const_iterator it;
        it = converters_.find(type);
        if (it != converters_.end()) { 
            // have already such a converter -> use it
            converter = it->second;
        }
        else { 
            // don't have it -> create it and store it for later use
            converter = BuildConverter(type);
            converters_[type] = converter;
        }
    }
    else{
        // a converter was specified. create it.
        // TODO: useful to create a converter cache for this case? probably not
        converter = BuildConverter(converterName);
    }
    // construct the table description
    I3TableRowDescriptionConstPtr ticDescription  = ticConverter_->GetDescription();
    I3TableRowDescriptionConstPtr convDescription = converter->GetDescription();

    if (tableName == "")
        tableName = name;

    // get the table from the service
    I3TablePtr table = ConnectTable(tableName, (*ticDescription | *convDescription) );

    // store all this in tables_ 
    TableBundle bundle;
    bundle.objectType = type;
    bundle.converter = converter;
    bundle.table = table;
    
	std::map<std::string,std::vector<TableBundle> >::iterator table_it = tables_.find(name);
	if (table_it == tables_.end()) {
		std::vector<TableBundle> tablelist(1,bundle);
		tables_[name] = tablelist;
	} else {
		table_it->second.push_back(bundle);
	}
    // tables_[name] = bundle;
}

/******************************************************************************/

// register one specific object, lazily. if type and converter are empty the writer 
// should figure out appropriate values
void I3TableWriter::AddObject(std::string name, std::string tableName, 
                              std::string type, I3ConverterPtr converter, 
                              I3FrameObjectConstPtr obj) {

    // create a converter if needed
    if (converter == NULL) {
        converter = BuildConverter(type);
    }

    // construct the table description
    I3TableRowDescriptionConstPtr ticDescription  = ticConverter_->GetDescription();
    I3TableRowDescriptionConstPtr convDescription = converter->GetDescription(obj);

    I3TableRowDescription combinedDescription = (*ticDescription | *convDescription);

    if (tableName == "")
        tableName = name;

	std::map<std::string,std::vector<TableBundle> >::iterator tlist_it = tables_.find(name);
	std::vector<TableBundle>::iterator t_it;
	bool duplicate = false;
	if (tlist_it != tables_.end()) {
		// a table is connected to the key already,
		// search for duplicates
		for (t_it = tlist_it->second.begin(); t_it != tlist_it->second.end(); ++t_it) {
			if (t_it->table->GetName() == tableName) {
				log_warn("There is already a table named '%s', appending superfluous underscores.",t_it->table->GetName().c_str());
				tableName += "_";
			}
			// by convention, identical description => identical converter
			if (combinedDescription == t_it->table->GetDescription()) {
				duplicate = true;
				log_error("Another table '%s' exists with the same description. The new table '%s' will be dropped.",
					t_it->table->GetName().c_str(),tableName.c_str());
				break;
			}
		}
	} else {
		tables_[name] = std::vector<TableBundle>();
		tlist_it = tables_.find(name);
	}
		
	if (!duplicate) {
		// get the table from the service
	    I3TablePtr table = ConnectTable(tableName, combinedDescription);

	    // store all this in tables_ 
	    TableBundle bundle;
	    bundle.objectType = type;
	    bundle.converter = converter;
	    bundle.table = table;
	
		tlist_it->second.push_back(bundle);
	}
}


/******************************************************************************/
        
void I3TableWriter::AddObject(std::string objName, TableSpec spec) {
    // FIXME: error/duplicate checking
    // wantedNames_.push_back(objName);
	tablespec_map::iterator n_it = wantedNames_.find(objName);
	if (n_it == wantedNames_.end()) {
		wantedNames_[objName] = std::vector<TableSpec>(1,spec);
	} else {
		n_it->second.push_back(spec);
	}
}

/******************************************************************************/

void I3TableWriter::AddType(std::string typeName, TableSpec spec) {
    // FIXME: error/duplicate checking
    // wantedTypes_.push_back(typeName);
	tablespec_map::iterator t_it = wantedTypes_.find(typeName);
	if (t_it == wantedTypes_.end()) {
		wantedTypes_[typeName] = std::vector<TableSpec>(1,spec);
	} else {
		t_it->second.push_back(spec);
	}
}

/******************************************************************************/
        
void I3TableWriter::AddConverter(std::string typeName, I3ConverterPtr converter) {
    // TODO add some error checking
    converters_[typeName] = converter;
}

/******************************************************************************/

void I3TableWriter::Setup() {
    // implemenation pending
}

/******************************************************************************/

void I3TableWriter::Convert(I3FramePtr frame) {
    // implemenation pending
    
    /*
    1. if types configured get list of object names and types from the frame
    2. for all configured names:
        convert
        (remove from list if list.size > 0)
    3. if list.size > 0 and types.size > 0:
        look in remaining objects if type fits
        add to configured objects
        convert
    */
    // TODO rethink requirement of event header? -> what about gcd frames?


    // I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(); // TODO name?
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(std::string("I3EventHeader"));

    // lazily initialize the header
    if (!ticConverter_->HasDescription()) { ticConverter_->GetDescription(header); }
    
    tablespec_map::iterator vlist_it;
    std::vector<TableSpec>::iterator v_it;
    std::vector<std::string>::const_iterator k_it; 
    std::map<std::string, std::vector<TableBundle> >::iterator tlist_it;
    std::vector<TableBundle>::iterator t_it;
    std::map<std::string, std::string>::iterator m_it;

    // loop over the wantedNames_ list and move objects that are in the frame 
    // to tables_. this loop erases elements from wantedNames_
    for(vlist_it = wantedNames_.begin(); vlist_it != wantedNames_.end(); ) {
        const std::string& objName = vlist_it->first;
        if( (tlist_it = tables_.find(objName)) == tables_.end() ) {
            // try to get the object from the frame 
           // if exist: derive type, get converter, add to tables_, pop from wanted list           
           I3FrameObjectConstPtr object = frame->Get<I3FrameObjectConstPtr>(objName);
           if(object) {
               const std::string& typeName = frame->typename_find(objName)->second;
               // std::string tableName = "";
               // std::string converterName = "";
               //            
               // if ( (m_it = objNameToTableName_.find(objName)) != objNameToTableName_.end()) {
               //     tableName = m_it->second;
               // }
               // if ( (m_it = typeNameToConverterName_.find(typeName)) != typeNameToConverterName_.end()) {
               //     converterName = m_it->second;
               // }
               
					for (v_it = vlist_it->second.begin(); v_it != vlist_it->second.end(); ++v_it) {
						AddObject(objName, v_it->tableName, typeName, v_it->converter, object);
					}
               // AddObject(objName, tableName, typeName, converterName);
               
               // erase returns an iterator to the following element
					// copy the iterator to avoid invalidating it on erase
					tablespec_map::iterator eraser = vlist_it++;
               wantedNames_.erase(eraser);
           }
           else{
               ++vlist_it;
           }
			} else { 
				++vlist_it; 
			} 
     }            

    // now, cycle through the typelist and look for objects in the frames that 
    // have the type and are not in tables_
    // TODO rather expensive loop -> restructure to optimize?
    std::vector<std::string> objectsInFrame = frame->keys();

    // for every type in wantedTypes_ ...
    for (vlist_it = wantedTypes_.begin(); vlist_it != wantedTypes_.end(); ++vlist_it) {
        const std::string& typeName = vlist_it->first;

        // .. loop through the frame
        for (k_it = objectsInFrame.begin(); k_it != objectsInFrame.end(); ++k_it) {
            const std::string& objName = *k_it;

            // if the object is already in tables ignore it 
            if ( tables_.find(objName) != tables_.end() )
                continue;

            // if the type matches get the object from the frame and call AddObject
            const std::string& objTypeName = 
                frame->typename_find(objName)->second;
            if (typeName == objTypeName) { 
                I3FrameObjectConstPtr object = frame->Get<I3FrameObjectConstPtr>(objName);
                if(object) { // should always be true
                    // std::string tableName = "";
                    // std::string converterName = "";

                    // if ( (m_it = objNameToTableName_.find(objName)) != objNameToTableName_.end()) {
                    //     tableName = m_it->second;
                    // }
                    // if ( (m_it = typeNameToConverterName_.find(objTypeName)) != typeNameToConverterName_.end()) {
                    //     converterName = m_it->second;
                    // }
                    
                    // AddObject(objName, tableName, objTypeName, converterName);
							for (v_it = vlist_it->second.begin(); v_it != vlist_it->second.end(); ++v_it) {
								AddObject(objName, v_it->tableName, typeName, v_it->converter, object);
							}
                }
                else {
                    log_fatal("frame->keys() yields keys that cannot be retrieved from the frame!");
                } // if(object)
            } // if (typeName == objTypeName)
        } // for (k_it
    } // for (vlist_it

    // now walk through tables_ and convert what is there
    for(tlist_it = tables_.begin(); tlist_it != tables_.end(); ++tlist_it) {	
        for(t_it = tlist_it->second.begin(); t_it!= tlist_it->second.end(); ++t_it) {
	
            const std::string& objName = tlist_it->first;
            const TableBundle& bundle = *t_it;
            
            I3FrameObjectConstPtr obj = frame->Get<I3FrameObjectConstPtr>(objName);
            if (!obj) {
                // TODO error logic
                // rows->Set<bool>("exists", false);
            }
            else {
	
                // ask the converter how many rows he will write
                unsigned int nrows = bundle.converter->GetNumberOfRows(obj);
            
                // with this information the table can create the rows
                I3TableRowPtr rows = bundle.table->CreateRow(nrows);
            
                // the converter can then fill them
                unsigned int rowsWritten = bundle.converter->Convert(obj, rows, frame);
            
                assert(rowsWritten == nrows);
            
                // fill the table index columns
                for (unsigned int i=0; i < nrows; ++i) {
                    rows->SetCurrentRow(i);
                    ticConverter_->Convert(header, rows, frame);
                    rows->Set<bool>("exists", true);
                }
                
                // hand the rows back to the table
                bundle.table->AddRow(header, rows);
            }
        } // for t_it
    } // for tlist_it


}

/******************************************************************************/

void I3TableWriter::Finish() {
    // TODO: create any pending (empty) tables... this would be all names remaining in wantedNames. but what type?

    // disconnect from all tables
    std::map<std::string, std::vector<TableBundle> >::iterator list_it;
    std::vector<TableBundle>::iterator it;
    for(list_it = tables_.begin(); list_it != tables_.end(); ++list_it) {
        for(it = list_it->second.begin(); it != list_it->second.end(); ++it) {
            DisconnectTable(it->table);
        }
    }
}

/******************************************************************************/

I3TablePtr I3TableWriter::ConnectTable(std::string tableName, 
                                       const I3TableRowDescription& description) {
    log_debug("Connecting to table %s", tableName.c_str());
    I3TableRowDescriptionPtr descPointer = 
        I3TableRowDescriptionPtr(new I3TableRowDescription(description));
    I3TablePtr table = service_->GetTable(tableName, descPointer);
    if (!table) {
        log_fatal("couldn't get table %s from the service", tableName.c_str());
    }
    table->SetConnectedToWriter(true);
    return table;
}

/******************************************************************************/

void I3TableWriter::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str());
    table->SetConnectedToWriter(false);
    table.reset();
}
