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

#include "hdf-writer/internals/I3TableRow.h"

/******************************************************************************/

void I3TableRow::init() {
    // assume nrows_ and description_ are set; data_ == 0
    const size_t totalChunkSize = nrows_ * description_->GetTotalChunkSize();
    const size_t totalByteSize = nrows_ * description_->GetTotalByteSize();
    data_ = new I3MemoryChunk[totalChunkSize];

    // initialize memory block with zeros - TODO useful?
    char* pointy = reinterpret_cast<char*>(&data_[0]);
    for (unsigned int i=0; i < totalByteSize; ++i)
        pointy[i] = 0;
}

/******************************************************************************/

I3TableRow::I3TableRow(I3TableRowDescriptionConstPtr description, 
                       unsigned int nrows) :
    description_(description),
    nrows_(nrows),
    currentRow_(0)
{
        init();
}



/******************************************************************************/

I3TableRow::~I3TableRow() {
    delete[] data_;
}

/******************************************************************************/

I3TableRow::I3TableRow(const I3TableRow& rhs) {
    description_ = rhs.GetDescription();
    nrows_ = rhs.GetNumberOfRows();
    currentRow_ = 0;
    
    size_t totalChunkSize = nrows_*rhs.GetDescription()->GetTotalChunkSize();
    size_t totalByteSize = nrows_*rhs.GetDescription()->GetTotalByteSize();
    data_ = new I3MemoryChunk[totalChunkSize];
    memcpy( data_, rhs.GetPointer(), totalByteSize );
}

/******************************************************************************/

I3TableRow& I3TableRow::operator=(const I3TableRow& rhs) {
    size_t totalByteSize = rhs.GetNumberOfRows()*rhs.GetDescription()->GetTotalByteSize();
    size_t totalChunkSize = rhs.GetNumberOfRows()*rhs.GetDescription()->GetTotalByteSize();
    if (totalByteSize == nrows_*description_->GetTotalByteSize()) {
        memcpy( data_, rhs.GetPointer(), totalByteSize );
    }
    else {
        delete[] data_;
        data_ = new I3MemoryChunk[totalChunkSize];
        memcpy( data_, rhs.GetPointer(), totalByteSize );
    }
    description_ = rhs.GetDescription();
    nrows_ = rhs.GetNumberOfRows();
    currentRow_ = 0;
    return *this;
}


/******************************************************************************/

void const* I3TableRow::GetPointer() const {
    return static_cast<void*>(data_); // TODO const?
}

/******************************************************************************/

I3TableRowDescriptionConstPtr I3TableRow::GetDescription() const {
    return description_;
}

/******************************************************************************/

unsigned int I3TableRow::GetNumberOfRows() const {
    return nrows_;
}

/******************************************************************************/

void I3TableRow::SetNumberOfRows(unsigned int nrows) {
    assert(description_); // FIXME change to log_fatal?
    
    if (nrows_ == nrows)
        return;

    nrows_ = nrows;
    currentRow_ = 0;
    delete[] data_;
    init();
}

/******************************************************************************/

void I3TableRow::SetCurrentRow(unsigned int row) {
    if ( (row < 0) || (nrows_ <= row) )
        log_fatal("try to set current row to %d outside of [0,%d)", row, nrows_);
    currentRow_ = row;
}

/******************************************************************************/

unsigned int I3TableRow::GetCurrentRow() {
    return currentRow_;
}

/******************************************************************************/

// For the times when you really want to blow your leg off...
template<>
void* I3TableRow::GetPointerToRow(const std::string& fieldName, unsigned int row) {
    int index;
    if ( (index = description_->GetFieldColumn(fieldName)) == -1 ) 
        log_fatal("trying to get the address of unknown field %s", fieldName.c_str());

    if ( !(( 0 <= row) && (row < nrows_)) )
        log_fatal("requested pointer to row %d which is not in [0,%d]", row, nrows_);
    
    /*
    if (description_->GetFieldArrayLengths().at(index) > 1 )
        log_fatal("trying to use I3TableRow::Get() on array element %s. Use GetPointer() instead",
                fieldName.c_str());
    */
    
    return ( reinterpret_cast<void*>( &data_[description_->GetTotalChunkSize()*row + 
                                             description_->GetFieldChunkOffsets().at(index)]) );
}

