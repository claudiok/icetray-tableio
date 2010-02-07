try:
	import tables
	have_pytables = True
except ImportError:
	have_pytables = False
	
if have_pytables:
	
	# Since we've extended the HDF5 high-level Table interface, we
	# have to provide our own convenience functions for PyTables
	def monkeypatch(target):
		def patcher(func):
			original = getattr(target, func.__name__)
			setattr(target, func.__name__, func)
			setattr(target, func.__name__ + '__unpatched', original)
			return func
		return patcher

	# Add access to units and docstrings at table level
	# FIXME (TODO): add _v_units or somesuch to tables.Description
	@monkeypatch(tables.Table)
	def _cacheDescriptionData(self):
		self._cacheDescriptionData__unapatched()
		ncols = len(self.description._v_names)
		self.colunits = [self._v_attrs.get('FIELD_%d_UNIT'%i,'') for i in xrange(ncols)]
		self.coldoc   = [self._v_attrs.get('FIELD_%d_DOC'%i, '') for i in xrange(ncols)]