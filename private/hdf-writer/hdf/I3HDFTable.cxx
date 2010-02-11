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

// If no description provided, read one from the table on disk
I3HDFTable::I3HDFTable(I3TableService& service, const std::string& name,
                       hid_t fileId, I3TablePtr index_table) :
    I3Table(service, name, I3TableRowDescriptionPtr()),
    fileId_(fileId) {
      indexTable_ = index_table;
      CreateDescription();
      
   }

/******************************************************************************/

// If a description is provided, create the corresponding table
I3HDFTable::I3HDFTable(I3TableService& service, const std::string& name,
                       I3TableRowDescriptionConstPtr description,
                       hid_t fileId, int compress, I3TablePtr index_table) :
    I3Table(service, name, description),
    fileId_(fileId) {
      indexTable_ = index_table;
      CreateTable(compress);
}

/******************************************************************************/

// create a table based on the TableRowDescription
void I3HDFTable::CreateTable(int& compress) {

    const size_t structSize = description_->GetTotalByteSize();
    const unsigned nfields = description_->GetNumberOfFields();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    const hid_t* fieldTypes    = &(description_->GetFieldHdfTypes().front());
  
    // converter field name strings into char**
    std::vector<const char*> fieldNameVector;
    const std::vector<std::string>& nameStrings = description_->GetFieldNames();
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
                       fieldTypes,         // field types
                       nchunks,         // write data in chunks of ...
                       NULL,            // fill data to be written at creation
                       compress,        // compress flag
                       NULL);              // data to be written at creation  
    if (status < 0) {
        log_fatal("Couln't create table");
        //TODO maybe just set tableCreated_=false and let the service handle the problem?
    }
    else {
        tableCreated_ = true;
    }

   // Add some helpful attributes
   const std::vector<std::string>& unitStrings = description_->GetFieldUnits();
   const std::vector<std::string>& docStrings  = description_->GetFieldDocStrings();
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

// Find the corresponding index table
// void I3HDFTable::FindIndexTable() {
//    
// }

/******************************************************************************/

// Create a TableRowDescription from an existing HDF table
void I3HDFTable::CreateDescription() {
   
   // for a table already on disk, we're responsible for the description
   I3TableRowDescriptionPtr description = I3TableRowDescriptionPtr(new I3TableRowDescription());
   
   // Get the dimensions of the table
   hsize_t nfields,nrecords,i;
   H5TBget_table_info(fileId_,name_.c_str(),&nfields,&nrecords);
   
   // grab the table's compound datatype
   hid_t dset_id = H5Dopen(fileId_,name_.c_str());
   hid_t table_dtype = H5Dget_type(dset_id);
   H5Dclose(dset_id);
   
   // this API is not so pretty. If I get the member name by hand, the library
   // mallocs enough memory to hold the string and hands me a pointer.
   // H5TBget_field_info does _exactly_ the same thing, only that it strcpys straight
   // into the buffer I've passed, size be damned. As far as I know, there's 
   // no simple/efficient way to query the size of the attribute string beforehand.
   char**  field_names   = new char*[nfields];
   for (i = 0; i < nfields; i++) {
      field_names[i] = H5Tget_member_name( table_dtype, (unsigned)i );
   }
   
   size_t* field_sizes   = new size_t[nfields];
   size_t* field_offsets = new size_t[nfields];
   size_t type_size;
      
   // FIXME: maybe I should just do this all by hand...
   H5TBget_field_info(fileId_,name_.c_str(),field_names,field_sizes,field_offsets,&type_size);
   
   description = I3TableRowDescriptionPtr(new I3TableRowDescription());
   
   hid_t dtype;
   int rank,perm;
   hsize_t array_size;
   
   std::string unit,doc;
   
   for (hsize_t i=0; i < nfields; i++) {
      dtype = H5Tget_member_type(table_dtype,i);
      
      if (H5Tget_class( dtype ) == H5T_ARRAY) {
         rank = H5Tget_array_ndims(dtype);
         if ( rank != 1 ) {
            log_fatal("Rank of array field /%s.%s is %d. That's deeply screwy!",name_.c_str(),field_names[i],rank);
         }
         H5Tget_array_dims(dtype,&array_size,&perm);
      } else {
         array_size = 1;
      }
      
      std::ostringstream osu,osd;
      osu << "FIELD_" << i << "_UNIT";
      osd << "FIELD_" << i << "_DOC";
      
      unit = ReadAttributeString(fileId_,name_,osu.str());
      doc  = ReadAttributeString(fileId_,name_,osd.str());

      // FIXME: set pytype as well
      description->AddField(std::string(field_names[i]),dtype,'\0',field_sizes[i],unit.c_str(),doc.c_str(),array_size);
      
      // don't release the datatype, as this will invalidate the ID
      // The I3TableRowDescription holding the ID should release it when it is destroyed
      // H5Tclose(dtype);
   }
   H5Tclose(table_dtype);
   
   // the individual strings were malloc'd by the HDF library,
   // not new'd by me
   for (i = 0; i < nfields; i++) free(field_names[i]);
   delete[] field_names;
   delete[] field_sizes;
   delete[] field_offsets;
   
   description_ = description;
   nrows_ = nrecords;
   tableCreated_ = true;

   //FIXME: fill lastHeader_, nevents_, etc.
}

/******************************************************************************/

// safely read an attribute string, avoiding overflows.
// This is sort of important, as we in principle allow entire PRLs
// to be pasted into the doc string for a field. 
std::string I3HDFTable::ReadAttributeString(hid_t fileID, std::string& where, std::string attribute) {
   
   size_t attr_size;
   hsize_t dims;
   H5T_class_t type_class;
   std::string val;
   herr_t status;
   
   status = H5LTget_attribute_info(fileID,where.c_str(),attribute.c_str(),&dims,&type_class,&attr_size);
   
   if ((status < 0) || (type_class != H5T_STRING)) {
      // printf("%s.%s: dims=%d, status=%d\n",where.c_str(),attribute.c_str(),(int)dims,status);
      return val;
   }
   
   char* attr_buffer = new char[attr_size];
      
   H5LTget_attribute_string(fileID,where.c_str(),attribute.c_str(),attr_buffer);
   
   val = attr_buffer;
   
   delete[] attr_buffer;
   return val;
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

I3TableRowConstPtr I3HDFTable::ReadRows(size_t start, size_t nrows) {
   if (!description_) {
      log_fatal("No I3TableRowDescription is set for this table.");
   }
   I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(description_,nrows));
   
   const size_t rowsize = description_->GetTotalByteSize();
   const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
   const size_t* fieldSizes   = &(description_->GetFieldTypeSizes().front());
   void* buffer = const_cast<void*>(rows->GetPointer());
   // herr_t status = 
       H5TBread_records(fileId_,        // file
                          name_.c_str(),  // data set name
                          start, // index at which to start reading
                          nrows, // number of records to read
                          rowsize,        // size of records in buffer
                          fieldOffsets,   // struct offsets
                          fieldSizes,     // size of the fields
                          buffer);        // where to write data
   return rows;
}

