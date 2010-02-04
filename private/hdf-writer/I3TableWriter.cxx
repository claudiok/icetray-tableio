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
    tables_[name] = bundle;
}

/******************************************************************************/
        
void I3TableWriter::AddObject(std::string objName) {
    // FIXME: error/duplicate checking
    wantedNames_.push_back(objName);
}

/******************************************************************************/

void I3TableWriter::AddType(std::string typeName) {
    // FIXME: error/duplicate checking
    wantedTypes_.push_back(typeName);
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

void I3TableWriter::Convert(I3FrameConstPtr frame) {
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
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(); // TODO name?

    std::vector<std::string>::iterator v_it;
    std::vector<std::string>::const_iterator k_it; 
    std::map<std::string, TableBundle>::iterator t_it;
    std::map<std::string, std::string>::iterator m_it;

    // loop over the wantedNames_ list and move objects that are in the frame 
    // to tables_. this loop erases elements from wantedNames_
    for(v_it = wantedNames_.begin(); v_it != wantedNames_.end();) {
        const std::string& objName = *v_it;
        if( (t_it = tables_.find(objName)) == tables_.end() ) {
            // try to get the object from the frame 
            // if exist: derive type, get converter, add to tables_, pop from wanted list
            I3FrameObjectConstPtr object = frame->Get<I3FrameObjectConstPtr>(objName);
            if(object) {
                const std::string& typeName = frame->typename_find(objName)->second;
                std::string tableName = "";
                std::string converterName = "";

                if ( (m_it = objNameToTableName_.find(objName)) != objNameToTableName_.end()) {
                    tableName = m_it->second;
                }
                if ( (m_it = typeNameToConverterName_.find(typeName)) != typeNameToConverterName_.end()) {
                    converterName = m_it->second;
                }
                
                AddObject(objName, tableName, typeName, converterName);
                
                // erase returns an iterator to the following element
                v_it = wantedNames_.erase(v_it);
            }
            else{
                ++v_it;
            }
        }
        else
            ++v_it;
    }
    // no cycle through the typelist and look for objects in the frames that 
    // have the type and are not in tables_
    // TODO rather expensive loop -> restructure to optimize?
    std::vector<std::string> objectsInFrame = frame->keys();

    // for every type in wantedTypes_ ...
    for (v_it = wantedTypes_.begin(); v_it != wantedTypes_.end(); ++v_it) {
        const std::string& typeName = *v_it;

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
                    std::string tableName = "";
                    std::string converterName = "";

                    if ( (m_it = objNameToTableName_.find(objName)) != objNameToTableName_.end()) {
                        tableName = m_it->second;
                    }
                    if ( (m_it = typeNameToConverterName_.find(objTypeName)) != typeNameToConverterName_.end()) {
                        converterName = m_it->second;
                    }
                    
                    AddObject(objName, tableName, objTypeName, converterName);
                }
                else {
                    log_fatal("frame->keys() yields keys that cannot be retrieved from the frame!");
                }
            }
        }
    }

    // now walk through tables_ and convert what is there
    for(t_it = tables_.begin(); t_it != tables_.end(); ++t_it) {
        const std::string& objName = t_it->first;
        const TableBundle& bundle = t_it->second;

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
            unsigned rowsWritten = bundle.converter->Convert(obj, rows, frame);

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

    }    
}

/******************************************************************************/

void I3TableWriter::Finish() {
    // TODO: create any pending (empty) tables... this would be all names remaining in wantedNames. but what type?

    // disconnect from all tables
    std::map<std::string, TableBundle>::iterator it;
    for(it = tables_.begin(); it != tables_.end(); ++it) {
        DisconnectTable(it->second.table);
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
}

/******************************************************************************/

void I3TableWriter::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str());
    table->SetConnectedToWriter(false);
    table.reset();
}
