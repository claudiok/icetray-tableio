/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#ifndef TABLEIO_I3MAPSTRINGCONVERTER_H_INCLUDED
#define TABLEIO_I3MAPSTRINGCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/I3Map.h"

class I3MapStringDoubleConverter : public I3ConverterImplementation<I3MapStringDouble> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3MapStringDouble& map);
    size_t FillRows(const I3MapStringDouble& map, I3TableRowPtr rows);
};

#endif // TABLEIO_I3MAPSTRINGCONVERTER_H_INCLUDED
