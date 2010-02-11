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
                   hid_t fileid, int compress, I3TablePtr index = I3TablePtr());

        I3HDFTable(I3TableService& service, const std::string& name,
                   hid_t fileId, I3TablePtr index = I3TablePtr());
        virtual ~I3HDFTable();
        static std::string ReadAttributeString(hid_t fileID, std::string& where, std::string attribute);
        virtual I3TableRowConstPtr ReadRows(size_t start, size_t nrows);

    protected:
        virtual void WriteRows(I3TableRowConstPtr row);
        void CreateTable(int& compress);
        void CreateDescription();
        hid_t fileId_;


    SET_LOGGER("I3HDFTable");
};
#endif
