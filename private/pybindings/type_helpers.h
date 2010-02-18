/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/internals/I3Datatype.h"

char PyArrayTypecode(const I3Datatype& dtype);

namespace I3Datatypes {
  enum NativeType {
    UNKNOWN,
    NATIVE_SCHAR,
    NATIVE_SHORT,
    NATIVE_INT,
    NATIVE_LONG,
    NATIVE_LLONG,
    NATIVE_UCHAR,
    NATIVE_USHORT,
    NATIVE_UINT,
    NATIVE_ULONG,
    NATIVE_ULLONG,
    NATIVE_FLOAT,
    NATIVE_DOUBLE,
    NATIVE_BOOL
  };
  NativeType GetNativeType(const I3Datatype& dtype);  
};

// Compatibility with Python < 2.4
#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif
