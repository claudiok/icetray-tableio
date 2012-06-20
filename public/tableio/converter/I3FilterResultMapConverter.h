/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision: 87998 $
 * @date $LastChangedDate: 2012-05-06 20:45:09 -0400 (Sun, 06 May 2012) $
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3FilterResult.h"

class I3FilterResultMapConverter : public I3ConverterImplementation<I3FilterResultMap> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3FilterResultMap& frmap);
        size_t FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows);
};
