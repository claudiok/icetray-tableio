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

#include "hdf-writer/internals/I3Table.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3TableRowDescription.h"
#include "hdf-writer/internals/I3TableService.h"

/******************************************************************************/

I3Table::I3Table(I3TableService& service, 
                 std::string name,
                 I3TableRowDescriptionConstPtr description) :
    service_(service),
    name_(name),
    description_(description),
    indexTable_(),
    nevents_(0),
    nrows_(0),
    nrowsWithPadding_(0),
    connected_(false),
    tableCreated_(false) {
    // implementation pending
}

/******************************************************************************/

I3Table::~I3Table() {}; // Flush?

/******************************************************************************/

bool I3Table::IsConnectedToWriter() {
    return connected_;
}

/******************************************************************************/

void I3Table::SetConnectedToWriter(bool connected) {
    connected_ = connected;
}

/******************************************************************************/
        
I3TableRowDescriptionConstPtr I3Table::GetDescription() {
    return description_;
}

/******************************************************************************/

void I3Table::AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row) {
    /* TODO add error checking;
       in principle this can break if the padding rows are too large
       (e.g. creation of a huge table at the end of a processing chain)
       maybe replace this with a while(padding = ...) loop
       and hand out rows in chunks 
    */
    assert(row->GetDescription() == description_);
    
    // sanity check: padding behavior is different for ragged tables
    unsigned int nrows = row->GetNumberOfRows(); 
    if ((nrows != 1) && (!description_->GetIsMultiRow())) {
        log_fatal("(%s) Converter reported %d rows for a single-row object! Multi-row objects must be marked by their converters.",name_.c_str(),nrows);
    }
    
    I3TableRowConstPtr padding;
    // only pad the data table itself if this is a non-ragged dataset
    if (!description_->GetIsMultiRow()) {
        padding = service_.GetPaddingRows(lastHeader_, header, description_);
        if (padding) {
            log_trace("(%s) Writing %u padding rows",name_.c_str(),padding->GetNumberOfRows());
            WriteRows(padding);
            nrowsWithPadding_ += padding->GetNumberOfRows();
        }
    } 
    // always pad the index table if it exists, since the index is alway a single row
    if (indexTable_) {
        padding = service_.GetPaddingRows(lastHeader_, header, service_.GetIndexDescription());
        if (padding) indexTable_->WriteRows(padding);
    }

    // FIXME: catch errors from the table
    WriteRows(row);
    
    if (indexTable_) {
      I3TableRowPtr index_row = I3TableRowPtr(new I3TableRow(service_.GetIndexDescription(),1));
      index_row->Set<unsigned int>("Run",header->GetRunID());
      index_row->Set<unsigned int>("Event",header->GetEventID());
      index_row->Set<bool>("exists",true);
      index_row->Set<unsigned int>("start",nrowsWithPadding_);
      index_row->Set<unsigned int>("stop",nrowsWithPadding_+row->GetNumberOfRows());
      log_trace("(%s) Writing row to index table. start: %d end: %d",
                name_.c_str(),nrowsWithPadding_,nrowsWithPadding_+row->GetNumberOfRows());
      
      indexTable_->WriteRows(index_row);
    }
    
    nevents_++;
    nrows_ += row->GetNumberOfRows();
    nrowsWithPadding_ += row->GetNumberOfRows();
    lastHeader_ = header;
    service_.HeaderWritten(lastHeader_,row->GetNumberOfRows());
}

/******************************************************************************/

// force the table to write out padding rows
// FIXME: this is triggered by passing a NULL header pointer
// is there a better way to do this?
// FIXME 2: this should only be called at the end, otherwise rows could be duplicated
void I3Table::Align() {
    I3TableRowConstPtr padding;
    // only pad the data table itself if this is a non-ragged dataset
    if (!description_->GetIsMultiRow()) {
        padding = service_.GetPaddingRows(lastHeader_, I3EventHeaderConstPtr(), description_);
        if (padding) {
            log_trace("(%s) Finalizing alignment with %u padding rows",name_.c_str(),padding->GetNumberOfRows());
            WriteRows(padding);
            nrowsWithPadding_ += padding->GetNumberOfRows();
        }
    }
    
    // always pad the index table if it exists, since the index is alway a single row
    if (indexTable_) {
        padding = service_.GetPaddingRows(lastHeader_, I3EventHeaderConstPtr(), service_.GetIndexDescription());
        if (padding) indexTable_->WriteRows(padding);
    }

    lastHeader_ = service_.GetLastHeader();
}

/******************************************************************************/

I3TableRowConstPtr I3Table::GetRowForEvent(unsigned int RunID, unsigned int EventID) {
	// FIXME: index implementation pending
	if (!indexTable_) { 
	    log_fatal("This table is write-only."); 
	} else {
	    return ReadRows(0,1);
	}
}

I3TableRowConstPtr I3Table::GetRowForEvent(unsigned int index) {
    std::pair<unsigned int,unsigned int> range;
    
    range = GetRangeForEvent(index);
    if (range.second == 0) {
        return I3TableRowPtr();
    } else {
        return ReadRows(range.first,range.second-range.first);
    }
}


std::pair<unsigned int,unsigned int> I3Table::GetRangeForEvent(unsigned int index) {
    log_fatal("This table is write-only.");
    return std::pair<unsigned int,unsigned int>();
}


/******************************************************************************/

// default implementation is write-only
I3TableRowConstPtr I3Table::ReadRows(size_t start, size_t nrows) {
	log_fatal("This table is write-only.");
	return I3TableRowConstPtr();
}


/******************************************************************************/

unsigned int I3Table::GetNumberOfEvents() const {
   return nevents_; 
}

/******************************************************************************/

unsigned int I3Table::GetNumberOfRows() const {
    return nrows_;
}

/******************************************************************************/

std::string I3Table::GetName() const {
    return name_;
}

/******************************************************************************/
        
I3TableRowPtr I3Table::CreateRow(unsigned int nrows) {
    return I3TableRowPtr( new I3TableRow(description_, nrows) );
}

/******************************************************************************/
