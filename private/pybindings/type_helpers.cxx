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

I3_POINTER_TYPEDEFS(I3Datatype);

namespace bp = boost::python;

I3DatatypePtr I3Datatype_from_Enum(bp::object enum_type);

// spit out an array.array-style type code
// FIXME: can we just make pybindings for I3Datatype?
char PyArrayTypecode_from_I3Datatype(const I3Datatype& dtype) {
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

boost::shared_ptr<I3Datatype> I3Datatype_from_PyObject(bp::object obj) {
    bp::extract<char> char_extractor(obj);
    if (char_extractor.check()) {
        return I3Datatype_from_PyArrayTypecode(char_extractor());
    }
    
    // FIXME: find a better way of recognizing a wrapped enum
    bp::object base = obj.attr("__base__").attr("__name__");
    std::string basename = bp::extract<std::string>(base);
    if (basename == std::string("enum")) {
        return I3Datatype_from_Enum(obj);
    } else {
        const std::string repr = bp::extract<const std::string>(bp::str(obj));
        std::string mesg = "Couldn't convert " + repr + " to an I3Datatype";
        PyErr_SetString(PyExc_TypeError,mesg.c_str());
        bp::throw_error_already_set();
        return I3DatatypePtr();
    }
};

enum DummyEnummy { foo };

I3DatatypePtr I3Datatype_from_Enum(bp::object enum_type) {
    boost::shared_ptr<I3Datatype> dtype(new I3Datatype);
    dtype->kind = I3Datatype::Enum;
    dtype->size = sizeof(DummyEnummy);
    dtype->is_signed = true;
    
    // extract values from the enum
    bp::tuple entry;
    std::string name;
    long value;
    bp::list dict_entries(enum_type.attr("values").attr("items")());
    bp::ssize_t n = bp::len(dict_entries);
    for (bp::ssize_t i = 0; i < n; i++) {
        entry = bp::extract<bp::tuple>(dict_entries[i]);
        value = bp::extract<long>(entry[0]);
        name = bp::extract<std::string>(entry[1].attr("name"));
        dtype->enum_members.push_back(std::make_pair(name,value));
    }
    
    dtype->description = bp::extract<std::string>(enum_type.attr("__name__"));
    return dtype;
};


boost::shared_ptr<I3Datatype> I3Datatype_from_PyArrayTypecode(char code) {
    char lower = std::tolower(code);
    boost::shared_ptr<I3Datatype> dtype(new I3Datatype);
    if (lower == 'b') {
        dtype->kind = I3Datatype::Int;
        dtype->size = sizeof(char);
        if (std::isupper(code)) dtype->is_signed = false;
        else dtype->is_signed = true;
    } else if (lower == 'h') {
        dtype->kind = I3Datatype::Int;
        dtype->size = sizeof(short);
        if (std::isupper(code)) dtype->is_signed = false;
        else dtype->is_signed = true;
    } else if (lower == 'i') {
        dtype->kind = I3Datatype::Int;
        dtype->size = sizeof(int);
        if (std::isupper(code)) dtype->is_signed = false;
        else dtype->is_signed = true;
    } else if (lower == 'l') {
        dtype->kind = I3Datatype::Int;
        dtype->size = sizeof(long);
        if (std::isupper(code)) dtype->is_signed = false;
        else dtype->is_signed = true;
    } else if (lower == 'f') {
        dtype->kind = I3Datatype::Float;
        dtype->size = sizeof(float);
        dtype->is_signed = true;
    } else if (lower == 'd') {
        dtype->kind = I3Datatype::Float;
        dtype->size = sizeof(double);
        dtype->is_signed = true;
    } else if (lower == 'o') {
        dtype->kind = I3Datatype::Bool;
        dtype->size = sizeof(bool);
        dtype->is_signed = true;
    }
    return dtype;
};

boost::shared_ptr<I3Datatype> I3Datatype_from_NumpyDtype(boost::python::object np_dtype) {
    I3DatatypePtr dtype(new I3Datatype());
    // sanity check on byte order
    char byteorder = PyString_AsString(bp::object(np_dtype.attr("byteorder")).ptr())[0];
    if (!(byteorder == '|') && !(byteorder == '=')) {
        // log_error("Array must be in native byte order (not '%c')",byteorder);
        return I3DatatypePtr();
    }
    char kind = PyString_AsString(bp::object(np_dtype.attr("kind")).ptr())[0];
    size_t size = bp::extract<size_t>(np_dtype.attr("itemsize"));
    dtype->size = size;
    switch (kind) {
        case 'i':
            dtype->kind = I3Datatype::Int;
            dtype->is_signed = true;
            break;
        case 'u':
            dtype->kind = I3Datatype::Int;
            dtype->is_signed = false;
            break;
        case 'f':
            dtype->kind = I3Datatype::Float;
            dtype->is_signed = true;
            break;
        case 'b':
            dtype->kind = I3Datatype::Bool;
            break;
        default:
            // return NULL for everything else
            dtype = I3DatatypePtr();
    }
    return dtype;
};

namespace I3Datatypes {
    using namespace I3Datatypes;
    NativeType GetNativeType(const I3Datatype& dtype) {
        NativeType type = UNKNOWN;
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