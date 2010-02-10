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

#include "hdf-writer/internals/I3TableService.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3ConverterFactory.h"

/******************************************************************************/

I3TableService::I3TableService()  { 
    ticConverter_ = BuildConverter("I3IndexColumnsGenerator");
}

/******************************************************************************/

std::vector<std::string> I3TableService::GetTableNames() {
	std::vector<std::string> out;
	out.reserve(tables_.size());
	std::map<std::string, I3TablePtr>::const_iterator it;
	
	for(it = tables_.begin(); it != tables_.end(); it++) out.push_back(it->first);
	return out;
}

/******************************************************************************/
        
I3TablePtr I3TableService::GetTable(std::string name,
                                    I3TableRowDescriptionConstPtr description) {
    std::map<std::string, I3TablePtr>::iterator it;
    I3TablePtr table;
    it = tables_.find(name);

    if (it != tables_.end()) {
        // test description 
        // TODO useful here? rethink task definitions
        // if (description->CanBeFilledInto( it->second->GetDescription() ) )
            table = it->second;
    } else if (description) {
       // create table if description is not NULL
       table = CreateTable(name,description);
       tables_[name] = table;
    }

    // otherwise, return NULL
    return table;
}

/******************************************************************************/
        
/*
I3TablePtr I3TableService::CreateTable(std::string tableName,
                                       I3TableRowDescriptionConstPtr description) {
    // implementation pending
    log_fatal("not implemented");
    return I3TablePtr();
}
*/

/******************************************************************************/
        
bool I3TableService::EventHeadersEqual(const I3EventHeader& header1,
                                       const I3EventHeader& header2) {
    //  ignore subeventid
    return ( (header1.GetRunID() == header2.GetRunID()) &&
             (header1.GetEventID() == header2.GetEventID()) );
}

/******************************************************************************/

// A table phones home to report that it has written a new event
void I3TableService::HeaderWritten(I3EventHeaderConstPtr lastHeader, unsigned int nrows) {
	if (lastHeader == NULL) return;
	
	else {
		if (eventHeaderCache_.size() == 0) {
			log_trace("First event: Run %d Event %d",lastHeader->GetRunID(),lastHeader->GetEventID());
			eventHeaderCache_.push_back(lastHeader);
		} else if ( !EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) && (nrows>0)) {
			log_trace("New event: Run %d Event %d",lastHeader->GetRunID(),lastHeader->GetEventID());
			eventHeaderCache_.push_back(lastHeader);
		}
	}
}


/******************************************************************************/
        
I3TableRowConstPtr I3TableService::GetPaddingRows(I3EventHeaderConstPtr lastHeader,
                                                  I3EventHeaderConstPtr newHeader,
                                             I3TableRowDescriptionConstPtr description_){
    // TODO change == into a comparison function

    // catch the cases where padding is not necessary 
    if (eventHeaderCache_.size() == 0) { // first call, first event 
        log_trace("Event header cache is empty, no padding required");
        return I3TableRowConstPtr();
    }

    unsigned nrows = 0;
    std::vector<I3EventHeaderConstPtr>::const_iterator it;

	if (!newHeader) log_fatal("newHeader is a NULL pointer!");
	else log_trace("newHeader: R%d E%d",newHeader->GetRunID(),newHeader->GetEventID());
	
	int i;
	for (it = eventHeaderCache_.begin(), i=0; it != eventHeaderCache_.end(); it++, i++) {
		log_trace("Cache %d: R%d E%d",i,(*it)->GetRunID(),(*it)->GetEventID());
	}   

	if (!lastHeader) { // this table is reporting for the first time
		if (EventHeadersEqual(*newHeader, *(eventHeaderCache_.back()))) {
			log_trace("This table is reporting for the first time; R%u E%u is the current event",newHeader->GetRunID(),newHeader->GetEventID());
			nrows = eventHeaderCache_.size()-1;
		} else {
			log_trace("This table is reporting for the first time; R%u E%u is a previous event",newHeader->GetRunID(),newHeader->GetEventID());
			nrows = eventHeaderCache_.size();
		}
	} else { // this table has written something in the past, so we search the cache for its last header
		if ( EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) ) { // first call for new event
	        log_trace("Table in sync with the cache, no padding required");
	        return I3TableRowConstPtr();
	   }
	
	 // padding is necessary -> figure out how many rows
    // 2 scenarios: l: lastHeader, h: newHeader, 1...6 cached headers
    // A: 1,2,3,l,4,5,6,h -> 3 padding rows
    // B: 1,2,3,l,4,5,(6=h) -> 2 padding rows
	
	 // scenario B
		if (EventHeadersEqual(*newHeader, *eventHeaderCache_.back())) { // another table already reported this event
			// start at 
			log_trace("Another table already reported this event.");
			for (it = eventHeaderCache_.begin(), i=0; it != eventHeaderCache_.end(); it++, i++) {
				if (*it == NULL) log_fatal("Element %d of the header cache (len %ld) is NULL!",i,eventHeaderCache_.size());
			}   
			
			for(it = eventHeaderCache_.end(); ((!EventHeadersEqual(**it, *lastHeader)) && (it != eventHeaderCache_.begin())); --it) nrows++;
			++it;  // go to the first missed event
			nrows -= 2;
		} else { // scenario A
			log_trace("This is the first report of this event, padding required.");   
			// for(it = eventHeaderCache_.end(); !EventHeadersEqual(**it, *lastHeader); --it) nrows++;
			for(it = eventHeaderCache_.end(); !EventHeadersEqual(**it, *lastHeader) && (it != eventHeaderCache_.begin()); --it) nrows++;
			++it; // go to the first missed event
			nrows -= 1;
		}
	}

	log_trace("nrows = %u",nrows);
    if (nrows <= 0) return I3TableRowConstPtr();
    I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(description_, nrows));
    I3FramePtr frame; // assume ticConv doesn't need the frame
    for (unsigned int i=0; i< nrows; ++i) {
        rows->SetCurrentRow(i);
        ticConverter_->Convert(*it, rows,frame);
        i++;
    }
    return rows;
}

/******************************************************************************/

void I3TableService::Finish() {
    // TODO: walk through tables, make sure all are disconnected and flush them
    CloseFile();
}

