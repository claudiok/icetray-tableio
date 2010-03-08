# hey look, i can a) write HDF5 files, and b) writer bookers in python!

from icecube import icetray,dataclasses,dataio,tableio
from I3Tray import I3Tray
from test_pybindings import DOMLaunchBookie

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


tray = I3Tray()

#from icecube import hdfwriter
from icecube import rootwriter
#from icecube import textwriter
#tabler = hdfwriter.I3HDFTableService('foo.hd5',0)
tabler = rootwriter.I3ROOTTableService('foo.root')
#tabler = textwriter.I3CSVTableService('foocsv')

tray.AddModule('I3Reader','reader',filename='/home/fabian/Physik/foo.i3.gz')
tray.AddModule(tableio.I3TableWriterModule,'writer',
    tableservice = tabler,
    keys  = [dict(key='LineFit', converter=I3ParticleConverter(), name='Way_out_there_beyond_this_hick_town_Barnaby')]
)

tray.AddModule('TrashCan','yeswecan')

tray.Execute(20)
tray.Finish()
