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

 namespace bp = boost::python;

void register_I3Datatype() {
    
    bp::scope dtype_scope = 
    bp::class_<I3Datatype>("I3Datatype")
        .def_readwrite("kind",&I3Datatype::kind)
        .def_readwrite("size",&I3Datatype::size)
        .def_readwrite("is_signed",&I3Datatype::is_signed)
    ;
    
    bp::enum_<I3Datatype::TypeClass>("TypeClass")
    #define TYPE_CLASS (Float)(Int)(Bool)(Enum)
    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, I3Datatype, TYPE_CLASS)
    ;
}