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
}

/******************************************************************************/

I3TableWriter::~I3TableWriter() {};

/******************************************************************************/

// register one specific object. if type and converter are empty the writer 
// should figure out appropriate values
void I3TableWriter::AddObject(std::string name, std::string tableName, 
                              std::string type, std::string converterName, 
                              const StringPairVector& params) {

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
            converter = BuildConverter(type, params); //, writer);
            converters_[type] = converter;
        }
    }
    else{
        // a converter was specified. create it.
        // TODO: useful to create a converter cache for this case? probably not
        converter = BuildConverter(converterName, params);
    }
    // construct the table description
    I3TableRowDescriptionConstPtr ticDescription  = ticConverter_->GetDescription();
    I3TableRowDescriptionConstPtr convDescription = converter->GetDescription();

    // get the table from the service
    I3TablePtr table = ConnectTable(tableName, (*ticDescription | *convDescription) );

    // store all this in tables_ 
    TableBundle bundle;
    bundle.objectType = type;
    bundle.converter = converter;
    bundle.table = table;
    tables_[name] = bundle;
}

// write all objects with this type
void I3TableWriter::AddType(std::string typeName, std::string converter,
                            const StringPairVector& params) {
    // implemenation pending
}

void I3TableWriter::Setup() {
    // implemenation pending
}

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
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(); // TODO name?
    
    std::map<std::string, TableBundle>::iterator it;
    for(it = tables_.begin(); it != tables_.end(); ++it) {
        const std::string& objName = it->first;
        const TableBundle& bundle = it->second;

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

void I3TableWriter::Finish() {
    // TODO: create any pending (empty) tables...

    // disconnect from all tables
    std::map<std::string, TableBundle>::iterator it;
    for(it = tables_.begin(); it != tables_.end(); ++it) {
        DisconnectTable(it->second.table);
    }
}

I3TablePtr I3TableWriter::ConnectTable(std::string tableName, 
                                       const I3TableRowDescription& description) {
    log_debug("Connecting to table %s", tableName.c_str());
    I3TableRowDescriptionPtr descPointer = 
        I3TableRowDescriptionPtr(new I3TableRowDescription(description));
    I3TablePtr table =  service_->GetTable(tableName, descPointer);
    table->SetConnectedToWriter(true);
}

void I3TableWriter::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str());
    table->SetConnectedToWriter(false);
    table.reset();
}
