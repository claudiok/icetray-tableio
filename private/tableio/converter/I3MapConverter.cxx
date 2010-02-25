/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3DoubleConverter.cxx 61800 2010-02-23 02:22:49Z jvansanten $
 *
 * @version $Revision: 61800 $
 * @date $LastChangedDate: 2010-02-22 21:22:49 -0500 (Mon, 22 Feb 2010) $
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy: jvansanten $
 */

//#include "tableio/I3Converter.h"
#include "tableio/internals/I3ConverterFactory.h"
#include "tableio/converter/I3MapConverter.h"

I3_CONVERTER(I3MapOMKeyConverter<I3DOMLaunchSeriesMap>, I3DOMLaunchSeriesMap);
I3_CONVERTER(I3MapOMKeyConverter<I3RecoHitSeriesMap>, I3RecoHitSeriesMap);
I3_CONVERTER(I3MapOMKeyConverter<I3RecoPulseSeriesMap>, I3RecoPulseSeriesMap);



