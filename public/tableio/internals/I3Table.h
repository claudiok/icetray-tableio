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

#ifndef	I3TABLE_H_INCLUDED
#define I3TABLE_H_INCLUDED

#include <string>

#include "icetray/IcetrayFwd.h"
//#include "tableio/internals/I3TableService.h"

I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3EventHeader);
I3_FORWARD_DECLARATION(I3TableService);

class I3Table {
    public:
        I3Table(I3TableService& service, std::string name,
                I3TableRowDescriptionConstPtr description);
        virtual ~I3Table(); // Flush?
        
        bool IsConnectedToWriter();
        void SetConnectedToWriter(bool connected);

        I3TableRowPtr CreateRow(unsigned int nrows);
        void AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row);
        void Align();
        
        I3TableRowConstPtr GetRowForEvent(unsigned int RunID, unsigned int EventID);
        I3TableRowConstPtr GetRowForEvent(unsigned int index);

        std::string GetName() const; 
        unsigned int GetNumberOfEvents() const; 
        unsigned int GetNumberOfRows() const; 
        I3TableRowDescriptionConstPtr GetDescription();

    protected:
        // to be overridden by derivatives
        virtual void WriteRows(I3TableRowConstPtr row) = 0;
        virtual I3TableRowConstPtr ReadRows(size_t start, size_t nrows);
        virtual std::pair<unsigned int,unsigned int> GetRangeForEvent(unsigned int index);

        I3TableService& service_;
        std::string name_;

        I3TableRowDescriptionConstPtr description_;
        boost::shared_ptr<I3Table> indexTable_;

        unsigned int nevents_; // number of events written
        unsigned int nrows_;   // number of rows (>= events) written
        unsigned int nrowsWithPadding_;   // number of rows (>= events) written, including padding
        bool connected_;       // connected to a writer
        bool tableCreated_;    // the table/tree has been created successfully

        I3EventHeaderConstPtr lastHeader_;
        
        enum AlignmentType {
            MultiRow,   // Some objects can span multiple rows
            Strict      // All objects are written to exactly one row
        };
        
        virtual AlignmentType GetAlignmentType();

    private:
        I3Table();

    SET_LOGGER("I3Table");

};

I3_POINTER_TYPEDEFS( I3Table );



#endif
