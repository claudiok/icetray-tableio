/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include <tableio/internals/I3Table.h>

class I3BroadcastTable : public I3Table {
    public:
        I3BroadcastTable(I3TableService& service, std::string name,
            I3TableRowDescriptionConstPtr description, std::vector<I3TablePtr>& clients);
        virtual void AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row);
        virtual void Align();
        
    private:
        void WriteRows(I3TableRowConstPtr rows);
        std::vector<I3TablePtr> clients_;
        
    SET_LOGGER("I3BroadcastTable");
};
