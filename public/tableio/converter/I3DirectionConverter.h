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

#ifndef TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED
#define TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/I3Direction.h"

class I3DirectionConverter : public I3ConverterImplementation<I3Direction>
{
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3Direction& direction);
        size_t FillRows(const I3Direction& direction, I3TableRowPtr row);
};

#endif // TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED
