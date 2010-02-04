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

#ifndef	I3HDFTABLE_H_INCLUDED
#define I3HDFTABLE_H_INCLUDED

#include "hdf-writer/internals/I3Table.h"
#include "H5TA.h"

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);

class I3HDFTable : public I3Table {
    public:
        I3HDFTable(I3TableService& service, const std::string& name,
                   I3TableRowDescriptionConstPtr description,
                   hid_t fileid, int compress);
        virtual ~I3HDFTable();
    protected:
        virtual void WriteRows(I3TableRowConstPtr row);
        hid_t fileId_;

};
#endif
