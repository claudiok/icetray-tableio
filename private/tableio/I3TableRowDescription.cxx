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

#include "tableio/internals/I3TableRowDescription.h"
#include <numeric>

/******************************************************************************/

I3TableRowDescription::I3TableRowDescription() : isMultiRow_(false) {}

/******************************************************************************/

I3TableRowDescription::~I3TableRowDescription() {}

/******************************************************************************/

const std::vector<std::string>& I3TableRowDescription::GetFieldNames() const {
    return fieldNames_;
}

const std::vector<I3Datatype>&  I3TableRowDescription::GetFieldTypes() const {
    return fieldTypes_;
}

// const std::vector<char>&  I3TableRowDescription::GetFieldTypeCodes() const {
//     return fieldTypeCodes_;
// }

const std::vector<size_t>& I3TableRowDescription::GetFieldTypeSizes() const {
    return fieldTypeSizes_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldArrayLengths() const {
    return fieldArrayLengths_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldByteOffsets() const {
    return fieldByteOffsets_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldChunkOffsets() const {
    return fieldChunkOffsets_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldUnits() const {
    return fieldUnits_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldDocStrings() const {
    return fieldDocStrings_;
}

/******************************************************************************/
        
size_t I3TableRowDescription::GetFieldColumn(const std::string& fieldName) const {
    std::map<std::string,size_t>::const_iterator iter;
    iter = fieldNameToIndex_.find(fieldName);
    if (iter != fieldNameToIndex_.end()) {
        return iter->second;
    }
    else {
        return GetNumberOfFields();
    }
}

/******************************************************************************/

bool I3TableRowDescription::CanBeFilledInto(I3TableRowDescriptionConstPtr other) const {
    size_t nfields = GetNumberOfFields();
    bool compatible = true;
    for (size_t i =0; i < nfields; ++i) {
        if ( (fieldNames_.at(i) != other->GetFieldNames().at(i) ) ||
             (fieldTypes_.at(i) != other->GetFieldTypes().at(i) ) ||
             (fieldTypeSizes_.at(i) != other->GetFieldTypeSizes().at(i) ) ) // redundant...
            compatible = false;
    }

    return compatible;
}

/******************************************************************************/

bool I3TableRowDescription::operator==(I3TableRowDescriptionConstPtr other) const {
   size_t nfields = GetNumberOfFields();
   size_t o_nfields = other->GetNumberOfFields();
   if (nfields != o_nfields) return false;
	if (isMultiRow_ != other->isMultiRow_) return false;
   bool equal = true;
   
   for (size_t i = 0; i < nfields; ++i) {
       if ( (fieldNames_.at(i) != other->GetFieldNames().at(i) ) ||
            (fieldTypes_.at(i) != other->GetFieldTypes().at(i) ) ||
            (fieldUnits_.at(i) != other->GetFieldUnits().at(i) ) ||
            (fieldDocStrings_.at(i) != other->GetFieldDocStrings().at(i) )          
          ) {
              equal = false;
              break;
            }
   }
   return equal;
}

/******************************************************************************/

    /* specialized AddField for booleans */
    template<>
    void I3TableRowDescription::AddField<bool>(const std::string& name, 
		  const std::string& unit,
		  const std::string& doc, 
		  size_t arrayLength) 
    {
		// Since booleans are just integers, we need to enforce this unit convention
		std::string boolunit("bool");
		if ((unit.size() != 0) && (unit != boolunit))
			log_fatal("The unit string of a boolean field must be \"bool\".");
		AddField(name, I3DatatypeFromNativeType<bool>(), sizeof(bool), 
			 boolunit, doc, arrayLength);
    }


/******************************************************************************/

void I3TableRowDescription::AddField(const std::string& name, I3Datatype type,
                                     size_t typeSize, const std::string& unit,
                                     const std::string& doc, size_t arrayLength) {

    size_t chunkOffset=0;
    size_t byteOffset=0;
    if (fieldChunkOffsets_.size() > 0) {
        chunkOffset = GetTotalChunkSize();
        byteOffset = GetTotalByteSize();
    }    
    size_t nfields = fieldNameToIndex_.size();
    fieldNames_.push_back(name);
    fieldNameToIndex_[name] = nfields;
    // if (arrayLength == 1)
    //     fieldHdfTypes_.push_back(hdfType);
    // else {
    //     hsize_t rank = 1; 
    //     std::vector<hsize_t> dims(1, arrayLength);
    //     hid_t array_tid = H5Tarray_create(hdfType, rank, &dims.front(), NULL);
    //     fieldHdfTypes_.push_back(array_tid);
    // }
    // special case for ambigous integers that may be booleans
    if ( unit == std::string("bool") ) { 
       // typeCode = 'o';
       type.kind = I3Datatype::Bool;
    }
    fieldTypes_.push_back(type);
       
    // else if (typeCode == 0) typeCode = py_code_from_hdf(hdfType);
    // fieldTypeCodes_.push_back(typeCode);
    fieldTypeSizes_.push_back(typeSize);
    fieldArrayLengths_.push_back(arrayLength);
    fieldChunkOffsets_.push_back(chunkOffset);
    fieldByteOffsets_.push_back(byteOffset);
    fieldUnits_.push_back(unit);
    fieldDocStrings_.push_back(doc);
}

/******************************************************************************/
        
size_t I3TableRowDescription::GetTotalChunkSize() const {
    // TODO catch no defined field case
    return fieldChunkOffsets_.back() + fieldArrayLengths_.back();
}

size_t I3TableRowDescription::GetTotalByteSize() const {
    // TODO catch no defined field case
    return I3MEMORYCHUNK_SIZE * GetTotalChunkSize();
}


/******************************************************************************/
        
size_t I3TableRowDescription::GetNumberOfFields() const {
    return fieldNames_.size(); 
}

/******************************************************************************/

I3TableRowDescription operator|(const I3TableRowDescription& lhs, 
                                const I3TableRowDescription& rhs) {
    I3TableRowDescription newlhs = I3TableRowDescription(lhs);
    size_t nfields = rhs.GetNumberOfFields();
    for (size_t i = 0; i < nfields; i++) {
        size_t byteOffset=0;
        size_t chunkOffset=0;
        if (newlhs.GetNumberOfFields() > 0)
            byteOffset = newlhs.GetTotalByteSize();
            chunkOffset = newlhs.GetTotalChunkSize();

        std::string fieldName = rhs.fieldNames_.at(i);

        newlhs.isMultiRow_ = (lhs.isMultiRow_ || rhs.isMultiRow_);
        
        // values that are just copied:
        newlhs.fieldNames_.push_back( fieldName );
        newlhs.fieldTypeSizes_.push_back( rhs.fieldTypeSizes_.at(i) );
        newlhs.fieldTypes_.push_back( rhs.fieldTypes_.at(i) );
        // newlhs.fieldTypeCodes_.push_back( rhs.fieldTypeCodes_.at(i) );
        newlhs.fieldArrayLengths_.push_back( rhs.fieldArrayLengths_.at(i) );
        newlhs.fieldUnits_.push_back( rhs.fieldUnits_.at(i) );
        newlhs.fieldDocStrings_.push_back( rhs.fieldDocStrings_.at(i) );
        
        // values that have to be recalculated:
        newlhs.fieldNameToIndex_[fieldName] = newlhs.fieldNameToIndex_.size()-1;
        newlhs.fieldByteOffsets_.push_back(byteOffset);
        newlhs.fieldChunkOffsets_.push_back(chunkOffset);
    }
    return newlhs;
}
        


