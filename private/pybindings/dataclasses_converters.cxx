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
#include "tableio/converter/I3MapConverter.h"
#include "tableio/converter/I3MapStringConverter.h"
#include "tableio/converter/I3ParticleConverter.h"
#include "tableio/converter/I3TreeConverter.h"
#include "tableio/converter/I3TriggerHierarchyConverter.h"
#include "tableio/converter/I3WaveformConverter.h"
#include "tableio/converter/PODConverter.h"
#include "tableio/converter/I3EventHeaderConverter.h"
#include "tableio/converter/I3VectorConverter.h"
#include "tableio/converter/I3WaveformSeriesMapConverter.h"

void register_dataclasses_converters() {
    I3CONVERTER_NAMESPACE(dataclasses);
    
    // typdef the template into a legal Python identifier
    I3CONVERTER_EXPORT(I3EventHeaderConverter,"Dumps I3EventHeader objects");
    
    typedef I3MapOMKeyVectorConverter<I3DOMLaunchSeriesMap> I3DOMLaunchSeriesMapConverter;
    I3CONVERTER_EXPORT(I3DOMLaunchSeriesMapConverter,"Dumps all DOMLaunches verbatim.");
    typedef I3MapOMKeyVectorConverter<I3RecoPulseSeriesMap> I3RecoPulseSeriesMapConverter;
    I3CONVERTER_EXPORT(I3RecoPulseSeriesMapConverter,"Dumps all RecoPulses verbatim.");
    typedef I3MapOMKeyVectorConverter<I3RecoHitSeriesMap> I3RecoHitSeriesMapConverter;
    I3CONVERTER_EXPORT(I3RecoHitSeriesMapConverter,"Dumps all RecoHits verbatim.");
    typedef I3MapOMKeyVectorConverter<I3MCHitSeriesMap> I3MCHitSeriesMapConverter;
    I3CONVERTER_EXPORT(I3MCHitSeriesMapConverter,"Dumps all MCHits verbatim.");
    
    I3CONVERTER_EXPORT(I3MapStringDoubleConverter,"Dumps a std::map<string,double> verbatim");
    I3CONVERTER_EXPORT(I3ParticleConverter,"Dumps an I3Particle verbatim");
    
    typedef I3TreeConverter<I3Particle> I3MCTreeConverter;
    I3CONVERTER_EXPORT(I3MCTreeConverter,"Dumps all particles in the MC Tree");
    
    I3CONVERTER_EXPORT(I3TriggerHierarchyConverter,"Dumps all triggers in the I3TriggerHierarchy tree");
    
    I3CONVERTER_EXPORT(I3DoubleConverter,"Dumps I3Double objects");
    I3CONVERTER_EXPORT(I3IntConverter,"Dumps I3Int objects");
    I3CONVERTER_EXPORT(I3BoolConverter,"Dumps I3Bool objects");

    typedef I3VectorConverter<bool> I3VectorBoolConverter;
    I3CONVERTER_EXPORT(I3VectorBoolConverter, "Dumps an I3Vector of bools");

    typedef I3VectorConverter<int16_t> I3VectorShortConverter;
    I3CONVERTER_EXPORT(I3VectorShortConverter, "Dumps an I3Vector of shorts");
    typedef I3VectorConverter<uint16_t> I3VectorUShortConverter;
    I3CONVERTER_EXPORT(I3VectorUShortConverter, "Dumps an I3Vector of unsigned shorts");

    typedef I3VectorConverter<int32_t> I3VectorIntConverter;
    I3CONVERTER_EXPORT(I3VectorIntConverter, "Dumps an I3Vector of ints");
    typedef I3VectorConverter<uint32_t> I3VectorUIntConverter;
    I3CONVERTER_EXPORT(I3VectorUIntConverter, "Dumps an I3Vector of unsigned ints");

    typedef I3VectorConverter<I3VectorInt64::value_type> I3VectorInt64Converter;
    I3CONVERTER_EXPORT(I3VectorInt64Converter, "Dumps an I3Vector of 64bit ints");
    typedef I3VectorConverter<I3VectorUInt64::value_type> I3VectorUInt64Converter;
    I3CONVERTER_EXPORT(I3VectorUInt64Converter, "Dumps an I3Vector of unsigned 64bit ints");

    typedef I3VectorConverter<float> I3VectorFloatConverter;
    I3CONVERTER_EXPORT(I3VectorFloatConverter, "Dumps an I3Vector of floats");
    typedef I3VectorConverter<double> I3VectorDoubleConverter;
    I3CONVERTER_EXPORT(I3VectorDoubleConverter, "Dumps an I3Vector of doubles");
  
    typedef I3VectorConverter< std::pair<double, double> > I3VectorDoubleDoubleConverter;
    I3CONVERTER_EXPORT(I3VectorDoubleDoubleConverter, "Dumps an I3Vector of double-double pairs");

    typedef I3VectorConverter<I3FlasherInfo> I3FlasherInfoVectConverter;
    I3CONVERTER_EXPORT(I3FlasherInfoVectConverter, "Dumps and I3FlasherInfoVect");

    // waveform converter has a non-default constructor
    // don't register this converter, since registered converters need default constructors
    bp::class_<I3WaveformConverter,                                      
    boost::shared_ptr<I3WaveformConverter>,                 
    bp::bases<I3Converter>,                      
    boost::noncopyable >("I3WaveformConverter", 
                         "Dumps ATWD and FADC waveforms together", 
                         bp::init<std::string, std::string, bool>())
    ;

    I3CONVERTER_EXPORT(I3WaveformSeriesMapConverter,
		       "Dumps a single I3WaveformSeriesMap (good for IceTop people not interested in FADC)")
      .def(bp::init<bool>());
};
