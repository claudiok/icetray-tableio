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

del I3Datatype,dt,make_dtype