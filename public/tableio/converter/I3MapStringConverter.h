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

#include "tableio/internals/I3ConverterFactory.h"
#include "dataclasses/I3Map.h"

class I3MapStringDoubleConverter : public I3ConverterImplementation<I3MapStringDouble> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3MapStringDouble& map);
    size_t FillRows(const I3MapStringDouble& map, I3TableRowPtr rows);
};
