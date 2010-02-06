from icecube import icetray,hdf_writer

class I3TableWriterModule(icetray.I3Module):
	def __init__(self,context):
		icetray.I3Module.__init__(self,context)
		self.AddParameter('TableService','The I3TableService to recieve output.',None)
		self.AddParameter('Keys','A list or dict of FrameObject keys to convert',None)
		self.AddParameter('Types','A list or dict of types to convert',None)
	def _get_tableservice(self):
		"""Get the table service (passed v3-style as a python object)"""
		table_service = self.GetParameter('TableService')
		if not isinstance(table_service, hdf_writer.I3TableService):
			raise TypeError, "TableService must be an instance of I3TableService (got %s instead)" % table_service
		self.table_service = table_service
	
	def _tuplify_args(self,arg,key_type,name):
		"""Turn {key_type: booker} dict, list of (key_type,booker) tuples, or list of key_type into list of tuples
		while checking that key_type is the proper type. booker == None means 'infer the booker for me.'"""
		if arg is None:
			return []
		if isinstance(arg,dict):
			arg = arg.items()
		elif isinstance(arg,list):
			if isinstance(arg[0],tuple):
				pass
			else:
				arg = [(k,None) for k in arg]
		else:
			raise TypeError, "%s must be a {%s: booker} dict, list of (%s:booker), or list of %s. (got %s instead)" % (name,key_type,key_type,key_type,arg)
		for k,v in arg:
			if not isinstance(k,key_type):
				raise TypeError, "%s must map %s -> booker; got %s as a key instead" % (name,key_type,k)
			if (v != None) and not isinstance(v,hdf_writer.I3Converter):
				raise TypeError, "%s must map %s -> booker; got %s as a booker instead" % (name,key_type,v)
		return arg
				
	def Configure(self):
		self._get_tableservice()
		
		# The grand configuration!
		keys = self.GetParameter('Keys')
		types = self.GetParameter('Types')
		empty = lambda t: (t is None) or (len(t) == 0)
		if empty(keys) and empty(types):
			raise ValueError, "You must specify either Keys or Types."
		if (not keys is None) and (not types is None):
			# raise ValueError, "You must specify either Keys or Types, but you may not specify both."
			# we will allow both, but configurations specified by key must take precedence
			pass
		
		# convert whatever was passed as 'Keys' to a list of tuples
		keys = self._tuplify_args(keys,str,'Keys')
		
		# convert whatever was passed as 'Types' to a list of tuples
		# FIXME (HACK): this returns <type 'Boost.Python.instance'>, but
		# there has to be a better way to check if something is a dataclass
		bp_instance = hdf_writer.I3Converter.__base__ 
		types = self._tuplify_args(types,object,'Types')
		
		# FIXME (HACK): this assumes that the pybindings classes are named
		# the same way as in the C++ I3FrameObject declaration, i.e. __name__
		# is the same thing that e.g. I3::name_of<I3Particle>() would return.
		# The pybindings type is in principle the most exact specification of
		# the type, so it would be preferable to use this.
		types = map(lambda t: (t[0].__name__,t[1]),types)
		
		self.writer = hdf_writer.I3TableWriter(self.table_service)
		tablespec = hdf_writer.I3TableWriter.TableSpec
		
		# FIXME: need a way to get table names from the user
		for key,converter in keys:
			if converter is None:
				self.writer.add_object(key,tablespec())
			else:
				self.writer.add_object(key,tablespec(converter))
		for type_,converter in types:
			if converter is None:
				self.writer.add_type(type_,tablespec())
			else:
				self.writer.add_type(type_,tablespec(converter))
	def Physics(self,frame):
		self.writer.convert(frame)
		self.PushFrame(frame)
		header = frame['I3EventHeader']
		print 'Run %d Event %d' % (header.RunID,header.EventID)
		return True
	def Finish(self):
		self.writer.finish()
		
		
if False == True:
	
	# A sketch of the interface
	
	from icecube import dataclasses
	from test_pybindings import DOMLaunchBookie
	service = hdf_writer.I3HDFTableService('foo.hdf',0)
	
	bookie = DOMLaunchBookie()
	
	tray.AddModule(I3HDFWriterModule,'hdfwriter',
			tableservice = service,
			keys = {'InIceRawData': bookie},
			types = {dataclasses.I3DOMLaunchSeriesMap: bookie}
			)
		