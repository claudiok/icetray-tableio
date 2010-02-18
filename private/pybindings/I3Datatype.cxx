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
#include <cctype>

namespace bp = boost::python;

boost::shared_ptr<I3Datatype> dtype_from_pycode(char code) {
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
}

void register_I3Datatype() {
    
    bp::scope dtype_scope = 
    bp::class_<I3Datatype, boost::shared_ptr<I3Datatype> >("I3Datatype")
        .def("__init__",bp::make_constructor(&dtype_from_pycode))
        .def_readwrite("kind",&I3Datatype::kind)
        .def_readwrite("size",&I3Datatype::size)
        .def_readwrite("is_signed",&I3Datatype::is_signed)
    ;
    
    bp::enum_<I3Datatype::TypeClass>("TypeClass")
    #define TYPE_CLASS (Float)(Int)(Bool)(Enum)
    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, I3Datatype, TYPE_CLASS)
    ;
}