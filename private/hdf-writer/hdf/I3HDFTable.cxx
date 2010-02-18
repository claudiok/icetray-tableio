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

#include <assert.h>

#include "hdf-writer/hdf/I3HDFTable.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3TableRowDescription.h"
#include "H5Tpublic.h"

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

// construct an HDF type from an I3Datatype
hid_t I3HDFTable::GetHDFType(const I3Datatype& dtype, const size_t arrayLength) {
    hid_t hdftype;
    std::vector<std::pair<std::string,long> >::const_iterator enum_it;
    switch (dtype.kind) {
        case I3Datatype::Bool:
            // do nothing
        case I3Datatype::Int:
            hdftype = H5Tcopy(H5T_NATIVE_INT);
            H5Tset_size(hdftype,dtype.size);
            if (dtype.is_signed) H5Tset_sign(hdftype,H5T_SGN_2);
            else H5Tset_sign(hdftype,H5T_SGN_NONE);
            break;
        case I3Datatype::Float:
            if (dtype.size == 4) {
                hdftype = H5Tcopy(H5T_NATIVE_FLOAT);
            } else if (dtype.size == 8) {
                hdftype = H5Tcopy(H5T_NATIVE_DOUBLE);
            } else if (dtype.size == 16) {
                hdftype = H5Tcopy(H5T_NATIVE_LDOUBLE);
            } else {
                log_fatal("I don't know what do with %ld-byte floats.",dtype.size);
            }
            break;
        case I3Datatype::Enum:
            hdftype = H5Tcopy(H5T_NATIVE_INT);
            H5Tset_size(hdftype,dtype.size);
            if (dtype.is_signed) H5Tset_sign(hdftype,H5T_SGN_2);
            else H5Tset_sign(hdftype,H5T_SGN_NONE);
            hid_t enumtype = H5Tenum_create(hdftype);
            H5Tclose(hdftype);
            hdftype = enumtype;
            for (enum_it = dtype.enum_members.begin(); enum_it != dtype.enum_members.end(); enum_it++) {
                H5Tenum_insert(hdftype,enum_it->first.c_str(),&(enum_it->second));
            }
            break;     
    }
    if (arrayLength > 1) {
        hsize_t rank = 1; 
        std::vector<hsize_t> dims(1, arrayLength);
        hid_t array_tid = H5Tarray_create(hdftype, rank, &dims.front(), NULL);
        H5Tclose(hdftype);
        hdftype = array_tid;
    }
    return hdftype;
}

/******************************************************************************/

// construct an I3Datatype from an HDF type
I3Datatype I3HDFTable::GetI3Datatype(hid_t hdftype, size_t* arrayLength ) {
    I3Datatype dtype;
    char* name;
    long value;
    hsize_t i,n,array_size;
    int rank,perm;
    
    switch (H5Tget_class(hdftype)) {
        case H5T_INTEGER:
            dtype.kind = I3Datatype::Int;
            dtype.size = H5Tget_size(hdftype);
            if (H5Tget_sign(hdftype) == H5T_SGN_2) dtype.is_signed = true;
            else dtype.is_signed = false;
            break;
        case H5T_FLOAT:
            dtype.kind = I3Datatype::Float;
            dtype.size = H5Tget_size(hdftype);
            dtype.is_signed = true;
            break;
        case H5T_ENUM:
            dtype.kind = I3Datatype::Enum;
            dtype.size = H5Tget_size(hdftype);
            n = H5Tget_nmembers(hdftype);
            dtype.enum_members.reserve(n);
            for (i = 0; i < n; i++) {
                name = H5Tget_member_name(hdftype,i);
                H5Tget_member_value(hdftype,i,&value);
                dtype.enum_members.push_back(std::make_pair(std::string(name),value));
                free(name);
            }
            break;
        case H5T_ARRAY:
            dtype = GetI3Datatype(H5Tget_super(hdftype),NULL);
            rank = H5Tget_array_ndims(hdftype);
            if (rank != 1) log_fatal("Rank of array is %d. This is deeply screwy!",rank);
            H5Tget_array_dims(hdftype,&array_size,&perm);
            *arrayLength = size_t(array_size);
            break;
        default:
            log_fatal("Unknown HDF type class %d.",(int)H5Tget_class(hdftype));
            break;
    }
    return dtype;
}

/******************************************************************************/

// create a table based on the TableRowDescription
void I3HDFTable::CreateTable(int& compress) {

    const size_t structSize = description_->GetTotalByteSize();
    const unsigned nfields = description_->GetNumberOfFields();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    
    // build HDF types from the supplied I3Datatypes
    // TODO: when should the hid_t's be freed?
    std::vector<hid_t> fieldHdfTypes;
    std::vector<I3Datatype>::const_iterator t_it;
    std::vector<size_t>::const_iterator arraySize_it;
    const std::vector<I3Datatype>& fieldI3Datatypes = description_->GetFieldTypes();
    const std::vector<size_t>& fieldArrayLengths = description_->GetFieldArrayLengths();
    for (t_it = fieldI3Datatypes.begin(), arraySize_it = fieldArrayLengths.begin();
         t_it != fieldI3Datatypes.end(); 
         t_it++, arraySize_it++) {
             fieldHdfTypes.push_back(GetHDFType(*t_it,*arraySize_it));
    }
    const hid_t* fieldTypes    = &(fieldHdfTypes.front());
  
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
   
   // set the MultiRow attribute
   char ragged = description_->GetIsMultiRow();
   H5LTset_attribute_char ( fileId_, name_.c_str(), "__I3RaggedTable__", &ragged, 1);
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
   
   hid_t hdftype;
   size_t array_size;
   std::string unit,doc;
   
   for (hsize_t i=0; i < nfields; i++) {
      hdftype = H5Tget_member_type(table_dtype,i);
      
      array_size = 1;
      I3Datatype dtype = GetI3Datatype(hdftype, &array_size);

      std::ostringstream osu,osd;
      osu << "FIELD_" << i << "_UNIT";
      osd << "FIELD_" << i << "_DOC";
      
      unit = ReadAttributeString(fileId_,name_,osu.str());
      doc  = ReadAttributeString(fileId_,name_,osd.str());

      // FIXME: set pytype as well
      description->AddField(std::string(field_names[i]),dtype,dtype.size,unit.c_str(),doc.c_str(),array_size);
      
      // don't release the datatype, as this will invalidate the ID
      // The I3TableRowDescription holding the ID should release it when it is destroyed
      // H5Tclose(dtype);
   }
   H5Tclose(table_dtype);
   
   // get the MultiRow attribute
   char ragged = 0;
   H5LTget_attribute_char ( fileId_, name_.c_str(), "__I3RaggedTable__", &ragged);
   description->isMultiRow_ = ragged;
   
   // the individual strings were malloc'd by the HDF library,
   // not new'd by me
   for (i = 0; i < nfields; i++) free(field_names[i]);
   delete[] field_names;
   delete[] field_sizes;
   delete[] field_offsets;
   
   description_ = description;
   nrows_ = nrecords;
   nrowsWithPadding_ = nrecords;
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

std::pair<unsigned int,unsigned int> I3HDFTable::GetRangeForEvent(unsigned int index) {
    if (indexTable_) {
        if (index > indexTable_->GetNumberOfRows()-1) {
            return std::pair<unsigned int,unsigned int>(0,0);
        }
        // FIXME: how to call ReadRows, which is a protected member of the parent class?
        boost::shared_ptr<I3HDFTable> indexTable = boost::static_pointer_cast<I3HDFTable>(indexTable_);
        I3TableRowPtr indexrow = boost::const_pointer_cast<I3TableRow>(indexTable->ReadRows(index,1));
        unsigned int start = indexrow->Get<unsigned int>("start");
        unsigned int end = indexrow->Get<unsigned int>("stop");
        return std::pair<unsigned int,unsigned int>(start,end);
    } else {
        log_fatal("(%s) This table has no index, and is thus write-only.",name_.c_str());
        return std::pair<unsigned int,unsigned int>();
    }
}

