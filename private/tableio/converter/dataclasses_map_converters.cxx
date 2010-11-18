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


/*
  // A compilation test: I3MapConverter should be able to accept any 
  // I3Converter as a template parameter. Remove comments to check.
  // Note: importing this would also require pybindings for I3Map<OMKey, vector<I3Particle> >
#include <tableio/converter/I3ParticleConverter.h>
  typedef I3Map<OMKey, std::vector<I3Particle> > I3MapKeyVectorI3Particle;
  I3_CONVERTER(I3MapOMKeyVectorConverter< I3ParticleConverter >, I3MapKeyVectorI3Particle);
*/
