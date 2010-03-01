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

namespace bp = boost::python;

// sets up a namespace "proj.converters" into which pybound converters
// can be exported. this allows you to e.g. export converters defined
// in tableio into the dataclasses module

#define I3CONVERTER_NAMESPACE(proj)                                                                                \
    bp::object module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj)))));\
    bp::object converter_module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj) ".converters"))));\
    module.attr("converters") = converter_module;\
    bp::dict converter_registry;\
    converter_module.attr("registry") = converter_registry;\
    bp::scope converter_scope = converter_module

// the minimal incantations to export pybindings for converters
// converter must be a legal python identifier (you can typedef this if necessary)

#define I3CONVERTER_EXPORT(converter,docstring)                    \
    register_converter<converter>(converter_registry,I3CONVERTER_EXPORT_IMPL(converter,docstring))

#define I3CONVERTER_EXPORT_IMPL(converter,docstring)               \
    bp::class_<converter,                                          \
               boost::shared_ptr<converter>,                       \
               bp::bases<I3Converter>,                             \
               boost::noncopyable >(BOOST_PP_STRINGIZE(converter),docstring)
    
// put the converter in a python-side (per-module) registry, then return the class
// scope for more method-adding.
template<typename Converter>
bp::scope register_converter(bp::dict& registry, bp::scope class_scope) {
    bp::object type = bp::object(typename Converter::booked_type()).attr("__class__");
    bp::list converters;
    if (registry.has_key(type)) {
        converters = bp::list(registry[type]);
    }
    converters.append(class_scope); 
    registry[type] = converters;
    return class_scope;
};



#endif  