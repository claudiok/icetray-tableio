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

#include "type_helpers.h"
#include <cctype>

// spit out an array.array-style type code
// FIXME: can we just make pybindings for I3Datatype?
char PyArrayTypecode(const I3Datatype& dtype) {
    char code = '\0';
    switch (dtype.kind) {
        case I3Datatype::Float:
            if (dtype.size == sizeof(float)) {
                code = 'f';
            } else if (dtype.size == sizeof(double)) {
                code = 'd';
            }
            break;
        case I3Datatype::Bool:
            code = 'o';
            break;
        case I3Datatype::Enum:
            // fall through
        case I3Datatype::Int:
            // FIXME: what to do about degenerate case 'c'?
            if (dtype.size == sizeof(char)) {
                code = 'b';
            } else if (dtype.size == sizeof(short)) {
                code = 'h';
            } else if (dtype.size == sizeof(int)) {
                code = 'i';
            } else if (dtype.size == sizeof(long)) {
                code = 'l';
            }
            if (!dtype.is_signed) code = std::toupper(code);
            break;
    }
    return code;
};