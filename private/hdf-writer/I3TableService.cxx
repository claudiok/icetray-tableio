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
        
I3TablePtr I3TableService::GetTable(std::string name,
                                    I3TableRowDescriptionConstPtr description) {
    std::map<std::string, I3TablePtr>::iterator it;
    I3TablePtr table;
    it = tables_.find(name);
    if (it != tables_.end()) {
        // test description 
        // TODO useful here? rethink task definitions
        if (description->CanBeFilledInto( it->second->GetDescription() ) )
            table = it->second;
    }

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
        
I3TableRowConstPtr I3TableService::GetPaddingRows(I3EventHeaderConstPtr lastHeader,
                                                  I3EventHeaderConstPtr newHeader,
                                             I3TableRowDescriptionConstPtr description_){
    // TODO change == into a comparison function

    // catch the cases where padding is not necessary 
    if (eventHeaderCache_.size() == 0) { // first call, first event 
        return I3TableRowConstPtr();
    }
    if ( EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) ) { // first call for new event
        return I3TableRowConstPtr();
    }
    // padding is necessary -> figure out how many rows
    // 2 scenarios: l: lastHeader, h: newHeader, 1...6 cached headers
    // A: 1,2,3,l,4,5,6,h -> 3 padding rows
    // B: 1,2,3,l,4,5,(6=h) -> 2 padding rows
    
    std::vector<I3EventHeaderConstPtr>::const_iterator it;
    unsigned nrows = 0;
    // scenario B
    if (EventHeadersEqual(*newHeader, *eventHeaderCache_.back())) { // another table already reported this event
        // start at 
        for(it = eventHeaderCache_.end(); !EventHeadersEqual(**it, *lastHeader); --it) 
            nrows++;
        ++it;  // go to the first missed event
        nrows -= 2;
    }
    // scenario A
    else {
        for(it = eventHeaderCache_.end(); !EventHeadersEqual(**it, *lastHeader); --it) 
            nrows++;
        ++it; // go to the first missed event
        nrows -= 1;
    }

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

