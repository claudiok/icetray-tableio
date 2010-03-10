.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$


Booking data in IceTray scripts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The IceTray interface to the framework is provided by a service that manages
the actual output file and a module that shuffles data between the frame and
the service. A simple script could look something like this::

    from icecube import icetray
    from I3Tray import I3Tray
    from icecube.tableio import I3TableWriter
    from icecube.hdfwriter import I3HDFTableService
    
    tray = I3Tray()
    tray.AddModule('I3Reader','reader',filename = 'foo.i3.gz')
    
    table_service = I3HDFTableService('foo.hd5')
    
    tray.AddModule(I3TableWriter,'writer',
                   tableservice = table_service,
                   keys         = ['LineFit','InIceRawData']
                  )
    
    tray.AddModule('TrashCan','yeswecan')
    tray.Execute()
    tray.Finish()

Here, the I3HDFTableService table_service is passed as a parameter to
I3TableWriter. The other parameter, 'keys', is a list of frame object
names. Any objects in the file named 'LineFit' will be written to a table
called 'LineFit' in the HDF5 file 'foo.hd5'.

Specifying objects by key
*************************

Objects can be specified by key. In addition, you may specify which converter should be used and what name the table should have in the output file. If you pass a list of keys::

    keys = ['LineFit','InIceRawData']

The frame objects called 'LineFit' and 'InIceRawData' will be written to the tables 'LineFit' and 'InIceRawData' using the default converters.

You can also pass a dictionary to specify which converters should be used::

    keys = {'LineFit': dataclasses.converters.I3ParticleConverter(), 
            'InIceRawData': dataclasses.converters.I3DOMLaunchSeriesMapConverter()}

For full control, you can pass a list of dicts, specifying the key, converter, and name of the output table::

    keys = [dict(key='LineFit', converter=dataclasses.converters.I3ParticleConverter(), name='SuperSpecialLineFit'),
            dict(key='InIceRawData', converter=dataclasses.converters.I3DOMLaunchSeriesMapConverter(), name='DOMLaunches')]



Specifying objects by type
**************************

If you don't know ahead of time which specific key-names you'd like to book, you can tell I3TableWriter to book all objects of a given type. Types are specified via their Python bindings::

    types = [dataclasses.I3Particle, dataclasses.I3DOMLaunchSeriesMap]

You can also specify converters to use for each type::

    types = {dataclasses.I3Particle: dataclasses.converters.I3ParticleConverter(),
             dataclasses.I3DOMLaunchSeriesMap: dataclasses.converters.I3DOMLaunchSeriesMapConverter()}

You can also pass a list of dicts specifying the type and converter::

    types = [dict(type=dataclasses.I3Particle, converter=dataclasses.converters.I3ParticleConverter(),
             dict(type=dataclasses.I3DOMLaunchSeriesMap, dataclasses.converters.I3DOMLaunchSeriesMapConverter())]

Booking everything in the file
*********************************

.. note::
    You should really avoid doing this whenever possible, as it's likely to create very, very large files.

If you really want dump *absolutely everything* in an I3 file using the
default converters, you can omit both the `Types` and `Keys` parameters::

    tray.AddModule(I3TableWriter,'writer', tableservice = table_service)

This gives you absolutely no control over how objects are booked. If you just
want to discover which converters are defined, you can inspect
:data:`tableio.I3ConverterRegistry.registry` in ipython. This dictionary
contains all the converters tableio knows about.



