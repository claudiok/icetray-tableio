# hey look, i can a) write HDF5 files, and b) writer bookers in python!

from icecube import icetray,dataclasses,dataio,hdf_writer
from I3Tray import I3Tray
from module import I3TableWriterModule
		
class I3ParticleConverter(hdf_writer.I3Converter):
	booked = dataclasses.I3Particle
	def CreateDescription(self,part):
		desc = hdf_writer.I3TableRowDescription()
		desc.add_field('x',      'd','m',     'x-position of particle')
		desc.add_field('y',      'd','m',     'y-position of particle')
		desc.add_field('z',      'd','m',     'z-position of particle')
		desc.add_field('time',   'd','ns',    'origin time of particle')
		desc.add_field('zenith', 'd','radian','zenith angle of particle origin')
		desc.add_field('azimuth','d','radian','azimuthal angle of particle origin')
		desc.add_field('energy', 'd','GeV',   'energy of particle')
		desc.add_field('speed',  'd','Gm/s',  'particle speed')
		desc.add_field('length', 'd','m',     'length of track')
		# skip: major_id/minor_id
		desc.add_field('type',      self.booked.ParticleType,'','')
		desc.add_field('shape',     self.booked.ParticleShape,'','')
		desc.add_field('location',  self.booked.LocationType,'','')
		desc.add_field('fit_status',self.booked.FitStatus,'','')
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

tabler = hdf_writer.I3HDFTableService('foo.hd5')

tray.AddModule('I3Reader','reader',filename='/Users/jakob/Documents/Wisc/2010 Spring/Python Primer/foo.i3.gz')
tray.AddModule(I3TableWriterModule,'writer',
	tableservice = tabler,
	# keys = {'LineFit': DummyBooker()},
	types = {dataclasses.I3Particle: I3ParticleConverter()}
)

tray.AddModule('TrashCan','yeswecan')

tray.Execute(20)
tray.Finish()
