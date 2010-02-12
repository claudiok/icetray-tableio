# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
#

try:
    import tables,numpy
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
        self._cacheDescriptionData__unpatched()
        ncols = len(self.description._v_names)
        # tables.attributeset.AttributeSet implements __getitem__, but not get(key,default)
        def get(item,key,default=None):
            try:
                return item[key]
            except KeyError:
                return default
        self.colunits = [get(self._v_attrs,'FIELD_%d_UNIT'%i,'') for i in xrange(ncols)]
        self.coldoc   = [get(self._v_attrs,'FIELD_%d_DOC'%i,'') for i in xrange(ncols)]
        ragged = get(self._v_attrs,'__I3RaggedTable__',None)
        if ragged is not None:
            self.ragged = (ragged[0] != 0)
        else:
            self.ragged = True # this is the more conservative assumption
        try:
            self.I3Index = self._v_file._getNode('/__I3Index__/%s' % self.name)
        except tables.NoSuchNodeError:
            self.I3Index = None
        
    def instancemethod(target):
        def adder(func):
            setattr(target,func.__name__,func)
            return func
        return adder
    
    @instancemethod(tables.Table)
    def readEvent(self,n):
        if self.I3Index is None:
            raise TypeError, "This table does not appear to have an index."
        limits = self.I3Index.read(n)
        if limits['start'] == limits['stop']:
            return numpy.empty(0,dtype=self.description._v_dtype)
        else:
            return self.read(limits['start'],limits['stop'])
    
        