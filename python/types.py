# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
# 

from icecube.tableio import I3Datatype

dt = I3Datatype.TypeClass
def make_dtype(kind, size, signed):
    dtype = I3Datatype()
    dtype.kind = kind
    dtype.size = size
    dtype.is_signed = signed
    return dtype

Int8    = make_dtype(dt.Int,1,True)
Int16   = make_dtype(dt.Int,2,True)
Int32   = make_dtype(dt.Int,4,True)
Int64   = make_dtype(dt.Int,8,True)
        
UInt8   = make_dtype(dt.Int,1,False)
UInt16  = make_dtype(dt.Int,2,False)
UInt32  = make_dtype(dt.Int,4,False)
UInt64  = make_dtype(dt.Int,8,False)

Float32 = make_dtype(dt.Float,4,True)
Float64 = make_dtype(dt.Float,8,True)

Bool    = make_dtype(dt.Bool,1,False)

# make a pretty-printer for I3Datatype
def pretty_dtype(self):
    return "I3Datatype(kind=%s, size=%s, signed=%s)"  % (self.kind,self.size,self.is_signed)
    
I3Datatype.__repr__ = pretty_dtype

class baseEnum(int):
	name = None
	values = {}
	def __repr__(self):
		return self.name

class metaEnum(type):
	"Helper metaclass to return the class variables as a dictionary "

	def __new__(cls, classname, bases, classdict):
		""" Return a new class with a "values" attribute filled
		"""

		newdict = {"values":{}}

		for k in classdict.keys():
			if not (k.startswith('_') or k == 'name' or k == 'values'):
				val = classdict[k]
				member = baseEnum(val)
				member.name = k
				newdict['values'][val] = member
				newdict[k] = member
		
		# Tell each member about the values in the enum
		for k in newdict['values'].keys():
			newdict['values'][k].values = newdict['values']
		# Return a new class with the "values" attribute filled
		return type.__new__(cls, classname, bases, newdict)

class Enum(baseEnum):
	"""This class mimicks the interface of boost-python-wrapped enums.
	
Inherit from this class to construct enumerated types that can
be passed to the I3Datatype, e.g.:

	class DummyEnummy(tableio.types.Enum):
		Foo = 0
		Bar = 1
		Baz = 2

	desc = tableio.I3TableRowDescription()
	desc.add_field('dummy', tableio.I3Datatype(DummyEnummy), '', '')
"""
	__metaclass__ = metaEnum

del I3Datatype,dt,make_dtype,pretty_dtype,baseEnum,metaEnum
