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

#include "tableio/internals/I3ConverterFactory.h"
#include "tableio/converter/I3MapConverter.h"

I3_CONVERTER(I3MapOMKeyVectorConverter<I3DOMLaunchSeriesMap>, I3DOMLaunchSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter<I3RecoPulseSeriesMap>, I3RecoPulseSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter<I3RecoHitSeriesMap>, I3RecoHitSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter<I3MCHitSeriesMap>, I3MCHitSeriesMap);

