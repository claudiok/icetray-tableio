/**
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Martin Wolf <martin.wolf@fysik.su.se> $LastChangedBy$
 */

#ifndef TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
#define TABLEIO_I3POSITIONCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/I3Position.h"


class I3PositionConverter : public I3ConverterImplementation<I3Position>
{
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3Position& position);
        size_t FillRows(const I3Position& position, I3TableRowPtr row);
};

#endif // TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
