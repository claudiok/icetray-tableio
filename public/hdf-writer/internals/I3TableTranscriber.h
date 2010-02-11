/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TableWriter.h 61608 2010-02-11 19:23:20Z jvansanten $
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
 */

#include <hdf-writer/internals/I3TableService.h>
#include <hdf-writer/internals/I3Table.h>

#ifndef	I3TABLETRANSCRIBER_H_INCLUDED
#define I3TABLETRANSCRIBER_H_INCLUDED

class I3TableTranscriber {
    public:
        I3TableTranscriber(I3TableServicePtr input, I3TableServicePtr output);
        void Execute();
        void Execute(unsigned int nframes);
        
        void Finish();
        
    protected:
         I3TablePtr ConnectTable(std::string tableName, 
                                 const I3TableRowDescription& description);
         void DisconnectTable(I3TablePtr& table);
    private:
 
        I3TableTranscriber();
        I3TableServicePtr inputService_;
        I3TableServicePtr outputService_;
        // std::map<std::string, I3TablePtr> inputTables_;
        std::vector<std::pair<I3TablePtr,I3TablePtr> > transcriptions_;
        unsigned int nEvents_;
    SET_LOGGER("I3TableTranscriber");
};

#endif

