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

#include "tableio/converter/I3MapConverter.h"
#include "tableio/converter/pybindings.h"

void register_dataclasses_converters() {
    I3CONVERTER_NAMESPACE(dataclasses);
    
    // typdef the template into a legal Python identifier
    typedef I3MapOMKeyVectorConverter<I3DOMLaunchSeriesMap> I3DOMLaunchSeriesMapConverter;
    I3CONVERTER_EXPORT(I3DOMLaunchSeriesMapConverter);
};
