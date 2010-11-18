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

#include "tableio/converter/pybindings.h"
#include "tableio/converter/dataclasses_map_converters.h"
#include "tableio/converter/dataclasses_vector_converters.h"
#include "tableio/converter/I3MapStringConverter.h"
#include "tableio/converter/I3ParticleConverter.h"
#include "tableio/converter/I3TreeConverter.h"
#include "tableio/converter/I3TriggerHierarchyConverter.h"
#include "tableio/converter/I3WaveformConverter.h"
#include "tableio/converter/PODConverter.h"
#include "tableio/converter/I3EventHeaderConverter.h"
#include "tableio/converter/I3WaveformSeriesMapConverter.h"

void register_dataclasses_converters() {
    I3CONVERTER_NAMESPACE(dataclasses);
    
    // typdef the template into a legal Python identifier
    I3CONVERTER_EXPORT_DEFAULT(I3EventHeaderConverter,"Dumps I3EventHeader objects");
    
    typedef I3MapOMKeyVectorConverter< convert::I3DOMLaunch > I3DOMLaunchSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3DOMLaunchSeriesMapConverter,"Dumps all DOMLaunches verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3RecoPulse > I3RecoPulseSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3RecoPulseSeriesMapConverter,"Dumps all RecoPulses verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3RecoHit > I3RecoHitSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3RecoHitSeriesMapConverter,"Dumps all RecoHits verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3MCHit > I3MCHitSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MCHitSeriesMapConverter,"Dumps all MCHits verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::pod<double> > I3MapKeyVectorDoubleConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorDoubleConverter, "Dumps all numbers verbatim");
    typedef I3MapOMKeyVectorConverter< convert::pod<int> > I3MapKeyVectorIntConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorIntConverter, "Dumps all numbers verbatim");
    
    I3CONVERTER_EXPORT_DEFAULT(I3MapStringDoubleConverter,"Dumps a std::map<string,double> verbatim");
    I3CONVERTER_EXPORT_DEFAULT(I3ParticleConverter,"Dumps an I3Particle verbatim");
    
    typedef I3TreeConverter<I3Particle> I3MCTreeConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3MCTreeConverter,"Dumps all particles in the MC Tree");
    
    I3CONVERTER_EXPORT_DEFAULT(I3TriggerHierarchyConverter,"Dumps all triggers in the I3TriggerHierarchy tree");
    
    I3CONVERTER_EXPORT_DEFAULT(I3DoubleConverter,"Dumps I3Double objects");
    I3CONVERTER_EXPORT_DEFAULT(I3IntConverter,"Dumps I3Int objects");
    I3CONVERTER_EXPORT_DEFAULT(I3BoolConverter,"Dumps I3Bool objects");

    typedef I3VectorConverter< convert::pod<bool> > I3VectorBoolConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorBoolConverter, "Dumps an I3Vector of bools");

    typedef I3VectorConverter< convert::pod<int16_t> > I3VectorShortConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorShortConverter, "Dumps an I3Vector of shorts");
    typedef I3VectorConverter< convert::pod<uint16_t> > I3VectorUShortConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUShortConverter, "Dumps an I3Vector of unsigned shorts");

    typedef I3VectorConverter< convert::pod<int32_t> > I3VectorIntConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorIntConverter, "Dumps an I3Vector of ints");
    typedef I3VectorConverter< convert::pod<uint32_t> > I3VectorUIntConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUIntConverter, "Dumps an I3Vector of unsigned ints");

    typedef I3VectorConverter< convert::pod<I3VectorInt64::value_type> > I3VectorInt64Converter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorInt64Converter, "Dumps an I3Vector of 64bit ints");
    typedef I3VectorConverter< convert::pod<I3VectorUInt64::value_type> > I3VectorUInt64Converter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUInt64Converter, "Dumps an I3Vector of unsigned 64bit ints");

    typedef I3VectorConverter< convert::pod<float> > I3VectorFloatConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorFloatConverter, "Dumps an I3Vector of floats");
    typedef I3VectorConverter< convert::pod<double> > I3VectorDoubleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorDoubleConverter, "Dumps an I3Vector of doubles");
  
    typedef I3VectorConverter< convert::double_pair > I3VectorDoubleDoubleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorDoubleDoubleConverter, "Dumps an I3Vector of double-double pairs");

    typedef I3VectorConverter< convert::I3FlasherInfo > I3FlasherInfoVectConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3FlasherInfoVectConverter, "Dumps and I3FlasherInfoVect");

    // waveform converter has a non-default constructor
    // don't register this converter, since registered converters need default constructors
    bp::class_<I3WaveformConverter,                                      
    boost::shared_ptr<I3WaveformConverter>,                 
    bp::bases<I3Converter>,                      
    boost::noncopyable >("I3WaveformConverter", 
                         "Dumps ATWD and FADC waveforms together", 
                         bp::init<std::string, std::string, bool>())
    ;

    I3CONVERTER_EXPORT_DEFAULT(I3WaveformSeriesMapConverter,
		       "Dumps a single I3WaveformSeriesMap (good for IceTop people not interested in FADC)")
      .def(bp::init<bool>());


/*
    // A compilation test: I3MapConverter and I3VectorConverter should be able
    // to accept any I3Converter as a template parameter. Remove comments to
    // check.
    typedef I3Map<OMKey, std::vector<I3Particle> > I3MapKeyVectorI3Particle;
    typedef I3MapOMKeyVectorConverter< I3ParticleConverter > I3MapKeyVectorI3ParticleConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorI3ParticleConverter, "Dumps all particles ... just a compilation test");
    
    typedef I3VectorConverter< I3ParticleConverter > I3VectorI3ParticleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorI3ParticleConverter, "Dumps an I3Vector<I3Particle>");
*/

};
