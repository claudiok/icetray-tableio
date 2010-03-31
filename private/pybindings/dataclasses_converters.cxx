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

void register_dataclasses_converters() {
    I3CONVERTER_NAMESPACE(dataclasses);
    
    // typdef the template into a legal Python identifier
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
    
    // waveform converter has a non-default constructor
    // don't register this converter, since registered converters need default constructors
    bp::class_<I3WaveformConverter,                                      
    boost::shared_ptr<I3WaveformConverter>,                 
    bp::bases<I3Converter>,                      
    boost::noncopyable >("I3WaveformConverter", 
                         "Dumps ATWD and FADC waveforms together", 
                         bp::init<std::string, std::string, bool>())
    ;
};
