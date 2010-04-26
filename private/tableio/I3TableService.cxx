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

#include "tableio/internals/I3TableService.h"
#include "tableio/internals/I3TableRow.h"
#include "tableio/internals/I3ConverterFactory.h"

/******************************************************************************/

I3TableService::I3TableService()  { 
    ticConverter_ = BuildConverter("I3IndexColumnsGenerator");
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<uint32_t>("Run", "", "run number");
    desc->AddField<uint32_t>("Event", "", "event number");
    desc->AddField<bool>("exists", "", "object was found in the frame");
    desc->AddField<tableio_size_t>("start", "", "Offset at which the rows for this event start");
    desc->AddField<tableio_size_t>("stop", "", "Offset at which the rows for the next event start");
    indexDescription_ = desc;
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
void I3TableService::HeaderWritten(I3EventHeaderConstPtr lastHeader, size_t nrows) {
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

// Get the last header the service has seen
I3EventHeaderConstPtr I3TableService::GetLastHeader() {
   return eventHeaderCache_.back();
}

/******************************************************************************/

// Get the last header the service has seen
I3TableRowDescriptionConstPtr I3TableService::GetIndexDescription() {
   return indexDescription_;
}


/******************************************************************************/
        
I3TableRowConstPtr I3TableService::GetPaddingRows(I3EventHeaderConstPtr lastHeader,
                                                  I3EventHeaderConstPtr newHeader,
                                             I3TableRowDescriptionConstPtr description_){
   // catch the cases where padding is not necessary 
   if (eventHeaderCache_.size() == 0) { // first call, first event 
     log_trace("Event header cache is empty, no padding required");
     return I3TableRowConstPtr();
   }

   size_t nrows = 0;
   std::vector<I3EventHeaderConstPtr>::reverse_iterator rit;

   if (!lastHeader) { // this table is reporting for the first time
      if (!newHeader) {
          log_trace("This table is reporting for the first time with a NULL header, padding out the table completely.");
          rit = eventHeaderCache_.rend()-1;
          nrows = eventHeaderCache_.size();
      } else if (EventHeadersEqual(*newHeader, *(eventHeaderCache_.back()))) {
         log_trace("This table is reporting for the first time; R%u E%u is the current event",newHeader->GetRunID(),newHeader->GetEventID());
         rit = eventHeaderCache_.rend()-1;
         nrows = eventHeaderCache_.size()-1;
      } else {
         log_trace("This table is reporting for the first time; R%u E%u is a previous event",newHeader->GetRunID(),newHeader->GetEventID());
         rit = eventHeaderCache_.rend()-1;
         nrows = eventHeaderCache_.size();
      }
   } else { // this table has written something in the past, so we search the cache for its last header
   
      // in sync, lastHeader is the same as the last entry in the cache
      if ( EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) ) { 
           log_trace("Table in sync with the cache, no padding required");
           return I3TableRowConstPtr();
      }
   
      // padding is necessary -> figure out how many rows
      // 2 scenarios: l: lastHeader, h: newHeader, 1...6 cached headers
      // A: 1,2,3,l,4,5,6,h -> 3 padding rows
      // B: 1,2,3,l,4,5,(6=h) -> 2 padding rows
      
      // scenario A (OR: newHeader NULL => table is finished, pad out all missing rows)
      if (!newHeader || !EventHeadersEqual(*newHeader, *(eventHeaderCache_.back()))) {
         log_trace("Padding up to last known event header");   
         for(rit = eventHeaderCache_.rbegin(); !EventHeadersEqual(**rit, *lastHeader) && rit != eventHeaderCache_.rend(); rit++) nrows++;
         --rit; // go to the first missed event (reverse iterator)
                // fill up to and including this event
      } else { // scenario B: another table already reported this event
         log_trace("Another table already reported this event.");
         // start at 
         for(rit = eventHeaderCache_.rbegin(); !EventHeadersEqual(**rit, *lastHeader) && rit != eventHeaderCache_.rend(); rit++) {
            nrows++;
         }
         --rit;  // go to the first missed event (reverse iterator)
         nrows -= 1; // fill up to but not including this event
      }

   }
      
   log_trace("nrows = %zu",nrows);
   if (nrows == 0) return I3TableRowPtr();
   I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(description_, nrows));
   I3FramePtr frame; // assume ticConv doesn't need the frame
   for (size_t i=0; i< nrows; ++i) {
      rows->SetCurrentRow(i);
      ticConverter_->Convert(*rit, rows,frame);
      rit--;
   }
   return rows;
}

/******************************************************************************/

void I3TableService::Finish() {
    // TODO: walk through tables, make sure all are disconnected and flush them
    CloseFile();
}

