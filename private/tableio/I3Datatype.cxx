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

// specialization for boolean types
template <>
I3Datatype I3DatatypeFromNativeType_impl<bool>(const char* label) {
    I3Datatype dtype;
    dtype.size = sizeof(bool);
    dtype.kind = I3Datatype::Bool;
    dtype.is_signed = false;
    dtype.description = label;
    return dtype;
};

namespace I3Datatypes {
    using namespace I3Datatypes;
    NativeType GetNativeType(const I3Datatype& dtype) {
        NativeType type = UKNOWN;
        switch (dtype.kind) {
            case I3Datatype::Int:
                if (dtype.size == sizeof(char)) {
                    if (dtype.is_signed) type = NATIVE_SCHAR;
                    else                 type = NATIVE_UCHAR;
                } else if (dtype.size == sizeof(short)) {
                    if (dtype.is_signed) type = NATIVE_SHORT;
                    else                 type = NATIVE_USHORT;
                } else if (dtype.size == sizeof(int)) {
                    if (dtype.is_signed) type = NATIVE_INT;
                    else                 type = NATIVE_UINT;
                } else if (dtype.size == sizeof(long)) {
                    if (dtype.is_signed) type = NATIVE_LONG;
                    else                 type = NATIVE_ULONG;
                } else if (dtype.size == sizeof(long long)) {
                    if (dtype.is_signed) type = NATIVE_LLONG;
                    else                 type = NATIVE_ULLONG;
                }
                break;
            case I3Datatype::Float:
                if (dtype.size == sizeof(float)) {
                    type = NATIVE_FLOAT;
                } else if (dtype.size == sizeof(double)) {
                    type = NATIVE_DOUBLE;
                }
                break;
            case I3Datatype::Bool:
                type = NATIVE_BOOL;
                break;
            default:
                break;
        }
        return type;
    }
};