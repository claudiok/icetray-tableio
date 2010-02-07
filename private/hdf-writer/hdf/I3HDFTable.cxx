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


#include "hdf-writer/hdf/I3HDFTable.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3TableRowDescription.h"

/******************************************************************************/

I3HDFTable::I3HDFTable(I3TableService& service, const std::string& name,
                       I3TableRowDescriptionConstPtr description,
                       hid_t fileId, int compress) :
    I3Table(service, name, description),
    fileId_(fileId) {

    const size_t structSize = description_->GetTotalByteSize();
    const unsigned nfields = description_->GetNumberOfFields();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    const size_t* fieldSizes   = &(description_->GetFieldTypeSizes().front());
    const hid_t* fieldTypes    = &(description_->GetFieldHdfTypes().front());
  
    // converter field name strings into char**
    std::vector<const char*> fieldNameVector;
    const std::vector<std::string>& nameStrings = description->GetFieldNames();
    std::vector<std::string>::const_iterator iter;
    for (iter = nameStrings.begin(); iter != nameStrings.end(); ++iter) {
        fieldNameVector.push_back(iter->c_str());
    }
    const char** fieldNames = static_cast<const char**>(&fieldNameVector.front());

    hsize_t nchunks = 100;

    herr_t status = 
        H5TBmake_table("",                 // TODO: table title -> add to interface
                       fileId_,            // hdf5 file opened by writer service 
                       name_.c_str(),      // name of the data set
                       nfields,            // number of table fiels
                       0,                  // number of records writen at creation
                       structSize,         // size of the struct
                       fieldNames,         // table field names
                       fieldOffsets,       // struct offsets
                       fieldTypes, 	       // field types
                       nchunks, 	       // write data in chunks of ...
                       NULL, 		       // fill data to be written at creation
                       compress,	       // compress flag
                       NULL);              // data to be written at creation  
    if (status < 0) {
        log_fatal("Couln't create table");
        //TODO maybe just set tableCreated_=false and let the service handle the problem?
    }
    else {
        tableCreated_ = true;
    }

   // Add some helpful attributes
   const std::vector<std::string>& unitStrings = description->GetFieldUnits();
   const std::vector<std::string>& docStrings  = description->GetFieldDocStrings();
   std::vector<std::string>::const_iterator it_unit;
   std::vector<std::string>::const_iterator it_doc;
   unsigned int i;
   // std::ostringstream osu,osd;
   for (i=0, it_unit = unitStrings.begin(), it_doc = docStrings.begin();
        it_unit != unitStrings.end();
        it_unit++, it_doc++, i++) {
         
         // FIXME: is there a kosher way to do this?
         std::ostringstream osu,osd;
         // build attribute names
         osu << "FIELD_" << i << "_UNIT";
         osd << "FIELD_" << i << "_DOC";

         // set string attributes on the dataset
         if (H5LTset_attribute_string( fileId_, name_.c_str(), osu.str().c_str(), it_unit->c_str() ) < 0)
               // FIXME: handle errors
               log_fatal("Couldn't set attribute '%s' = '%s' for dataset '%s'",
                          osu.str().c_str(),it_unit->c_str(),name_.c_str());
         if (H5LTset_attribute_string( fileId_, name_.c_str(), osd.str().c_str(), it_doc->c_str() ) < 0)
               // FIXME: handle errors
               log_fatal("Couldn't set attribute '%s' = '%s' for dataset '%s'",
                          osd.str().c_str(),it_doc->c_str(),name_.c_str());
      
   }
   
}

/******************************************************************************/

I3HDFTable::~I3HDFTable() {}

/******************************************************************************/

void I3HDFTable::WriteRows(I3TableRowConstPtr rows) {
    const unsigned int nrows = rows->GetNumberOfRows();
    const size_t rowsize = description_->GetTotalByteSize();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    const size_t* fieldSizes   = &(description_->GetFieldTypeSizes().front());
    const void* buffer = rows->GetPointer();
    herr_t status = 
        H5TBappend_records(fileId_,        // file
                           name_.c_str(),  // data set name
                           nrows,	       // number of the records in buffer
                           rowsize,        // size of records in buffer
                           fieldOffsets,   // struct offsets
                           fieldSizes, 	   // size of the fields
                           buffer);		   // data to write
    if (status < 0) {
        // TODO: better error handling. maybe return the number of successfull
        // written rows and let the service complain if something goes wrong
        log_fatal("failed to append rows to table.");
    }
}
