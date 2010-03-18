/**
 * Helper macros for I3Converter pybindings
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#ifndef PYBINDINGS_H_GNEDBL5C
#define PYBINDINGS_H_GNEDBL5C

#include "tableio/internals/I3Converter.h"
#include <boost/python/class.hpp>

namespace bp = boost::python;

// sets up a namespace "proj.converters" into which pybound converters
// can be exported. this allows you to e.g. export converters defined
// in tableio into the dataclasses module

#define I3CONVERTER_NAMESPACE(proj)                                                                                \
    bp::object module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj)))));\
    bp::object converter_module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj) ".converters"))));\
    module.attr("converters") = converter_module;\
    bp::object tableio(bp::handle<>(PyImport_Import(bp::str("icecube.tableio").ptr())));\
    bp::object registry(tableio.attr("I3ConverterRegistry"));\
    bp::scope converter_scope = converter_module

// the minimal incantations to export pybindings for converters
// converter must be a legal python identifier (you can typedef this if necessary)

#define I3CONVERTER_EXPORT(converter,docstring)                    \
    register_converter<converter>(registry,I3CONVERTER_EXPORT_IMPL(converter,docstring))
    

#define I3CONVERTER_EXPORT_IMPL(converter,docstring)               \
    bp::class_<converter,                                          \
               boost::shared_ptr<converter>,                       \
               bp::bases<I3Converter>,                             \
               boost::noncopyable >(BOOST_PP_STRINGIZE(converter),docstring)
    
// put the converter in a python-side registry, then return the class
// scope for more method-adding.
template<typename Converter,class W,class X1,class X2,class X3> // template args for bp::class_
bp::class_<W,X1,X2,X3> register_converter(bp::object& registry, bp::class_<W,X1,X2,X3> classy) {
    bp::object type = bp::object(typename Converter::booked_type()).attr("__class__");
    bp::scope class_scope = classy;
    registry.attr("register")(class_scope,type);
    return classy;
};



#endif  