.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$


Writing and extending converters
========================================

tableio makes it particularly easy to write converters for FrameObjects defined in your own project. These can be written either in Python or C++.

A converter must complete two tasks:

1. Create a table description based on a frame object. This includes the data
type, name, unit, and docstring of each field, as well as the number of
elements in the field if it should store an array.

2. Fill a table structure with data extracted from the frame object. This will
be passed to the table writer service, which interprets it according to the
description and writes it out to disk. The data may come directly from the frame object or be the result of a calculation (e.g. the number of entries in a DOMLaunchSeriesMap, which can be useful for identifying high-energy events).

Best practices
*****************

First, there are a few conventions you should follow to make the output of your converter useful.

Field names
_______________

Your field names should be all lower-case, separated by underscores, like 'raw_atwd_0', 'fit_status', or 'zenith'. 

Units
_______________

*Always* include unambiguous units in your table descriptions. Even if you're using the standard I3Units (e.g. gigameters per second for velocity), you should document it.

Docstrings
_________________

Include a useful documentation string whenever applicable. This is especially important if there is *any* possible ambiguity about how the field's value was calculated or should be interpreted. 

Data types
________________

In Python, you use the predefined types (e.g. tableio.types.Float32 or
tableio.types.UInt8) to specify the type of the field. For enumerated types,
you can create an I3Datatype from a boost-python-wrapped enum as shown in the
example below.

.. highlight:: c++

In C++, you have slightly more freedom and thus more responsibility. In order
to prevent the output of your converter from depending on the architecture and
operating system on which it is compiled, you should use the C99 fixed-length
integer types rather than the C89 integers, whose length is
implementation-defined. If you require a 64-bit unsigned integer, write::

    descPtr->AddField<uint64_t>("far","m","Distance to Markarian 501")

rather than the ambiguous::

    descPtr->AddField<unsigned long>("far","m","Distance to Markarian 501")

which will create a 32-bit field on 32-bit systems and a 64-bit field on
64-bit systems.


Writing a converter for your FrameObject
========================================

The easy way
************

.. highlight:: python

It is pretty simple to implement a converter in Python. All you need to do is
define a class that inherits from I3Converter and implements two methods,
CreateDescription() and FillRows(), and defines a class-level attribute
'booked' holding the type the converter expects. Here, for example, is an
implementation of an I3Particle converter in Python::

    class I3ParticleConverter(tableio.I3Converter):
        booked = dataclasses.I3Particle
        def CreateDescription(self,part):
            desc = tableio.I3TableRowDescription()
            desc.add_field('x',      tableio.types.Float64,'m',     'x-position of particle')
            desc.add_field('y',      tableio.types.Float64,'m',     'y-position of particle')
            desc.add_field('z',      tableio.types.Float64,'m',     'z-position of particle')
            desc.add_field('time',   tableio.types.Float64,'ns',    'origin time of particle')
            desc.add_field('zenith', tableio.types.Float64,'radian','zenith angle of particle origin')
            desc.add_field('azimuth',tableio.types.Float64,'radian','azimuthal angle of particle origin')
            desc.add_field('energy', tableio.types.Float64,'GeV',   'energy of particle')
            desc.add_field('speed',  tableio.types.Float64,'Gm/s',  'particle speed')
            desc.add_field('length', tableio.types.Float64,'m',     'length of track')
            # skip: major_id/minor_id
            desc.add_field('type',      tableio.I3Datatype(self.booked.ParticleType),'','')
            desc.add_field('shape',     tableio.I3Datatype(self.booked.ParticleShape),'','')
            desc.add_field('location',  tableio.I3Datatype(self.booked.LocationType),'','')
            desc.add_field('fit_status',tableio.I3Datatype(self.booked.FitStatus),'','')
            return desc
        def Convert(self,particle,row,frame):
            row['x']          = particle.x
            row['y']          = particle.y
            row['z']          = particle.z
            row['time']       = particle.time
            row['zenith']     = particle.zenith
            row['azimuth']    = particle.azimuth
            row['energy']     = particle.energy
            row['speed']      = particle.speed
            row['length']     = particle.length
            row['type']       = particle.type
            row['shape']      = particle.shape
            row['location']   = particle.location_type
            row['fit_status'] = particle.fit_status
            return 1

The converter expects a FrameObject of type dataclasses.I3Particle and produces an I3TableRowDescription with 13 fields. The fields 'type', 'shape', 'location', and 'fit_status' are filled with enumerated types. The labels associated with the enum members will be stored in the final file.

You can put this class definition in the Python directory associated with your project and use the converter in your steering files without introducing a hard dependency on tableio. 

Extending an existing converter
***********************************

It is even simpler to add additional fields to an existing converter. Suppose that for a particular analysis, we wish to extend the I3Particle converter to book celestial coordinates in addition to detector coordinates. We would define a new converter for I3Particle that looks something like this::

    class SkyBooker(tableio.I3Converter):
        """Demo of a booker extension, e.g. to book celestial coordinates"""
        def CreateDescription(self,part):
            desc = tableio.I3TableRowDescription()
            desc.add_field('RA', tableio.types.Float64,'radian','right ascension')
            desc.add_field('Dec',tableio.types.Float64,'radian','declination')
            desc.add_field('galactic_latitude (epoch 2000)', tableio.types.Float64,'radian','Galactic latitude (epoch 2000)')
            desc.add_field('galactic_longitude',tableio.types.Float64,'radian','Galactic longitude')
            return desc
        def Convert(self,particle,row,frame):
            """Here, we would have to get the event time from the header
            In order to transform from detector to celestial coords."""
            # coordinate-service magic
            coord = coordinate_service.Coordinate(frame['I3EventHeader'],particle)
            row['RA'] = coord.RA
            row['Dec'] = coord.Dec
            row['galactic_latitude'] = coord.GalacticLatitude
            row['galactic_longitude'] = coord.GalacticLongitude
        
            return 1

We can then pass a *list* of converters to I3TableWriter instead of a single converter instance::

        types = {dataclasses.I3Particle: [I3ParticleConverter(), SkyBooker()]}

Each converter will get a chance to fill the table structure with the appropriate data.

The hard (but perhaps slightly faster) way
******************************************

.. highlight:: c++

You can also define your converters in C++. To do this, you need to inherit
from the templated C++ class I3ConverterImplementation<YourFrameObject>. Here,
for example, is the header for a converter for I3FilterResultMap from
jebclasses, which would live in
public/jebclasses/converter/I3FilterResultMap.h::

    class I3FilterResultMapConverter : public I3ConverterImplementation<I3FilterResultMap> {
        private:
            I3TableRowDescriptionPtr CreateDescription(const I3FilterResultMap& frmap);
            unsigned int FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows);
    };

and the implementation, in private/converter/I3FilterResultMap.cxx::

    #include "jebclasses/converter/I3FilterResultMapConverter.h"

    I3TableRowDescriptionPtr I3FilterResultMapConverter::CreateDescription(const I3FilterResultMap& frmap) {
        I3TableRowDescriptionPtr desc = 
            I3TableRowDescriptionPtr(new I3TableRowDescription() );
        I3FilterResultMap::const_iterator it;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            desc->AddField<bool>(it->first,"bool","Field 0: condition passed, Field 1: prescale passed",2);
        }
        return desc;
    }
        
    unsigned int I3FilterResultMapConverter::FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows) {
        I3FilterResultMap::const_iterator it;
        bool* filter_result;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            filter_result = rows->GetPointer<bool>(it->first);
            filter_result[0] = it->second.conditionPassed;
            filter_result[1] = it->second.prescalePassed;
        }
        return 1;
    }

    I3_CONVERTER(I3FilterResultMapConverter, I3FilterResultMap);

The call to the I3_CONVERTER() macro registers I3FilterResultMapConverter as
the default converter for objects of type I3FilterResultMap.

If you want users to be able to specify your converter by name, you have to
write trivial pybindings for it. The tableio project provides preprocessor
macros that automate most of this. Here are example pybindings for
I3FilterResultMapConverter::

    #include "jebclasses/converter/I3FilterResultMapConverter.h"
    #include "tableio/converter/pybindings.h"
    
    void register_I3Converters() {
        I3CONVERTER_NAMESPACE(jebclasses);
        I3CONVERTER_EXPORT(I3FilterResultMapConverter);
    }

The macro I3CONVERTER_NAMESPACE(jebclasses) sets up a Python module
icecube.jebclasses.converters into which pybindings for
I3FilterResultMapConverter are exported.

