from icecube import icetray,hdf_writer
import re

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
	
	def _transform_keyitem(self,item):
		if isinstance(item,tuple):
			if len(item) == 1:
				return dict(key=item[0])
			elif len(item) == 2:
				return dict(key=item[0],converter=item[1])
			elif len(item) == 3:
				return dict(key=item[0],converter=item[1],name=item[2])
			else:
				raise ValueError, "Keys must be 2- or 3-tuples (got %s instead)" % item
		elif isinstance(item,str):
			return dict(key=item)
		elif isinstance(item,dict):
			#sanitize your inputs
			dictus = dict()
			for k in ['key','converter','name']:
				if item.has_key(k): dictus[k] = item[k]
			return dictus
		else:
			raise TypeError, "Keys must be dicts, tuples, or strings."
		
	def _transform_typeitem(self,item):
		typus = None
		dictus = None
		if isinstance(item,tuple):
			if len(item) == 1:
				dictus = dict(type=item[0])
			elif len(item) == 2:
				dictus = dict(type=item[0],converter=item[1])
			else:
				raise ValueError, "Types must be 1- or 2-tuples (got %s instead)" % item
		elif isinstance(item,dict):
			#sanitize your inputs
			dictus = dict()
			for k in ['type','converter']:
				if item.has_key(k): dictus[k] = item[k]
		else:
			dictus = dict(type=item)
		typus = dictus['type']
		if not isinstance(typus,str):
			# FIXME (HACK): this assumes that the pybindings classes are named
			# the same way as in the C++ I3FrameObject declaration, i.e. __name__
			# is the same thing that e.g. I3::name_of<I3Particle>() would return.
			# The pybindings type is in principle the most exact specification of
			# the type, so it would be preferable to use this.
			try:
				name = typus.__name__
				dictus['type'] = name
			except AttributeError:
				raise TypeError, "Couldn't interpret '%s' as a type-specification." % typus
		return dictus
				
	def _parse_args(self,arg,transformer):
		if arg is None:
			return []
		if isinstance(arg,dict):
			arg = arg.items()
		if isinstance(arg,list):
			arg = [transformer(item) for item in arg]
		else:
			raise TypeError, "Arguments must be passed as something list-like."
		valid_name = re.compile("^[a-zA-Z_][a-zA-Z0-9_]*$") 
		for item in arg:
			converter = item.get('converter',None)
			if not converter is None:
				if not isinstance(converter,hdf_writer.I3Converter):
					raise TypeError, "In '%s': converter must be an instance of I3Converter"
			name = item.get('name',None)
			if not name is None:
				if not valid_name.match(name):
					raise ValueError, "'%s' is not a valid table name. Table names must contain only letters, numbers, and underscores and may not start with a number" % name	
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
		
		# convert whatever was passed as 'Keys' to a list of dicts
		keys = self._parse_args(keys,self._transform_keyitem)
		
		# convert whatever was passed as 'Types' to a list of dicts
		types = self._parse_args(types,self._transform_typeitem)
		
		self.writer = hdf_writer.I3TableWriter(self.table_service)
		tablespec = hdf_writer.I3TableWriter.TableSpec
		
		for item in keys:
			key = item.pop('key')
			self.writer.add_object(key,tablespec(**item))
		for item in types:
			typus = item.pop('type')
			self.writer.add_type(typus,tablespec(**item))

	def Physics(self,frame):
		self.writer.convert(frame)
		self.PushFrame(frame)
		# header = frame['I3EventHeader']
		# print 'Run %d Event %d' % (header.RunID,header.EventID)
		return True
	def Finish(self):
		self.writer.finish()
		
		
if False == True:
	
	# A sketch of the interface
	
	from icecube import dataclasses
	from test_pybindings import DOMLaunchBookie
	service = hdf_writer.I3HDFTableService('foo.hdf',0)
	
	bookie = DOMLaunchBookie()
	alt_bookie = DOMLaunchBookie(do_magic = True)
	
	#like this:
	tray.AddModule(I3HDFWriterModule,'hdfwriter',
			tableservice = service,
			keys = [dict(key='InIceRawData', converter=bookie, name='HickTown_Barnaby'),
					  dict(key='InIceRawData', converter=alt_bookie, name='SlickTown_Barnaby')],
			types = {dataclasses.I3DOMLaunchSeriesMap: bookie}
			)
		