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