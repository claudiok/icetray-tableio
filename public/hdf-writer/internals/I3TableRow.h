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

#ifndef	I3_TABLEROW_H_INCLUDED
#define I3_TABLEROW_H_INCLUDED

// icetray includes
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "hdf-writer/internals/I3TableRowDescription.h"
#include "hdf-writer/internals/I3MemoryChunk.h"

class I3TableRow {
    public:
        I3TableRow(I3TableRowDescriptionConstPtr description, unsigned int nrows=1);
        I3TableRow(const I3TableRow& rhs);
        I3TableRow& operator=(const I3TableRow& rhs);

        virtual ~I3TableRow(); // TODO measure gain by making the destructor not virtual
        
        // set the current row one which following Set and Get calls operate
        void SetCurrentRow(unsigned int row);
        unsigned int GetCurrentRow();
        
        // set the value of a field
        template<class T>
        void Set(const std::string& fieldName, T value, bool all);

        // set the value of a field in the current row
        template<class T>
        void SetCurrent(const std::string& fieldName, T value);
        
        // get the value of a field in the current row
        template<class T>
        T Get(const std::string& fieldName);

        // set field with name fieldName to value for all rows
        template<class T>
        void SetAll(const std::string& fieldName, T value);
        
        // get a pointer to the beginning of field for the current row
        template<class T>
        T* GetPointer(const std::string& fieldName);
        
        // get a pointer to the beginning of field for the given row
        template<class T>
        T* GetPointerToRow(const std::string& fieldName, unsigned int row);

        // get a void pointer to whole memory block
        void const* GetPointer() const;
        I3TableRowDescriptionConstPtr GetDescription() const;

        unsigned int GetNumberOfRows() const;
        void SetNumberOfRows(unsigned int nrows);


    private:
        I3TableRow();
        void init();

        I3TableRowDescriptionConstPtr description_;
        unsigned int nrows_;
        unsigned int currentRow_;
        I3MemoryChunk* data_;

};

I3_POINTER_TYPEDEFS( I3TableRow );

/******************************************************************************/

template<class T>
void I3TableRow::Set(const std::string& fieldName, T value, bool all = false) {
	if (all) SetCurrent(fieldName,value);
	else SetAll(fieldName,value);
}

template<class T>
void I3TableRow::SetCurrent(const std::string& fieldName, T value) {
    T*  pointer = GetPointerToRow<T>(fieldName, currentRow_); 
    *pointer = value;
}

template<class T>
void I3TableRow::SetAll(const std::string& fieldName, T value) {
    for (unsigned int row = 0; row < nrows_; ++row) {
       T*  pointer = GetPointerToRow<T>(fieldName, row); 
       *pointer = value;
    }
}

/******************************************************************************/
        
template<class T>
T I3TableRow::Get(const std::string& fieldName) {
    return *GetPointerToRow<T>(fieldName, currentRow_);
}

/******************************************************************************/

template<class T>
T* I3TableRow::GetPointer(const std::string& fieldName) {
    return GetPointerToRow<T>(fieldName, currentRow_);
}

template<class T>
T* I3TableRow::GetPointerToRow(const std::string& fieldName, unsigned int row) {
    int index;
    if ( (index = description_->GetFieldColumn(fieldName)) == -1 ) 
        log_fatal("trying to get the address of unknown field %s", fieldName.c_str());

    if (sizeof(T) != description_->GetFieldTypeSizes().at(index) )
        log_fatal("size mismatch between the requested type and field");

    if ( !(( 0 <= row) && (row < nrows_)) )
        log_fatal("requested pointer to row %d which is not in [0,%d]", row, nrows_);
    
    /*
    if (description_->GetFieldArrayLengths().at(index) > 1 )
        log_fatal("trying to use I3TableRow::Get() on array element %s. Use GetPointer() instead",
                fieldName.c_str());
    */
    
    //return ( reinterpret_cast<T*>( &data_[description_->GetFieldOffsets().at(index)]) );
    return ( reinterpret_cast<T*>( &data_[description_->GetTotalChunkSize()*row + 
                                          description_->GetFieldChunkOffsets().at(index)]) );
}

#endif
