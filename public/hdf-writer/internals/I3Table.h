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

I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3EventHeader);
I3_FORWARD_DECLARATION(I3TableService);

class I3Table {
    public:
        I3Table(const I3TableService& service, std::string name,
                I3TableRowDescriptionConstPtr description, 
                unsigned int initialRows=0);
        ~I3Table(); // Flush?

        void Create();
        void Align();
        
        bool IsAligned();
        bool IsConnectedToWriter();
        void SetConnectedToWriter(bool connected);

        I3TableRowPtr CreateRow(unsigned int nrows);
        void AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row);

        

        std::string GetName() const; 
        unsigned int GetNumberOfEvents() const; 
        unsigned int GetNumberOfRows() const; 
        I3TableRowDescriptionConstPtr GetDescription();

    protected:
        const I3TableService& service_;
        std::string name_;

        I3TableRowDescriptionConstPtr description_;
        boost::shared_ptr<I3Table> indexTable_;

        unsigned int nevents_; // number of events written
        unsigned int nrows_;   // number of rows (>= events) written
        bool connected_;       // connected to a writer

    private:
        I3Table();

};

I3_POINTER_TYPEDEFS( I3Table );



#endif
