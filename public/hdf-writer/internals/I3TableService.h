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

#ifndef	I3TABLESERVICE_H_INCLUDED
#define I3TABLESERVICE_H_INCLUDED

#include <string>

#include "icetray/IcetrayFwd.h"

#include "dataclasses/physics/I3EventHeader.h"
#include "hdf-writer/internals/I3Table.h"

I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3Table);

class I3TableService {
    public:
        I3TableService();

        I3TablePtr GetTable(std::string tableName, 
                            I3TableRowDescriptionConstPtr description);

        void Finish();

    private:
        std::map<std::string, I3TablePtr> tables_;

        std::vector<I3EventHeader> eventHeaderCache_;
        // to be overridden by implementation
        I3TablePtr CreateTable(std::string tableName, 
                               I3TableRowDescriptionConstPtr description);
};

I3_POINTER_TYPEDEFS( I3TableService );



#endif
