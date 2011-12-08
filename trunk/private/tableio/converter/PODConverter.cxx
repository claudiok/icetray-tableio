/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3DoubleConverter.cxx 61920 2010-02-26 17:35:43Z jvansanten $
 *
 * @version $Revision: 61920 $
 * @date $LastChangedDate: 2010-02-26 18:35:43 +0100 (Fr, 26. Feb 2010) $
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy: jvansanten $
 */

#include "tableio/converter/PODConverter.h"
#include "tableio/converter/I3STLContainerConverter.h"

I3_CONVERTER(I3DoubleConverter, I3Double);
I3_CONVERTER(I3IntConverter, I3Int);
I3_CONVERTER(I3BoolConverter, I3Bool);
I3_CONVERTER(I3STLVectorConverter<I3Double>, std::vector<I3Double>);
