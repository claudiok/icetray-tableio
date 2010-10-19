/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/I3ConverterFactory.h"
#include "dataclasses_map_converters.h"

I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3DOMLaunch >, I3DOMLaunchSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3RecoPulse >, I3RecoPulseSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3RecoHit >, I3RecoHitSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3MCHit >, I3MCHitSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::pod<double> >, I3MapKeyVectorDouble);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::pod<int> >, I3MapKeyVectorInt);
