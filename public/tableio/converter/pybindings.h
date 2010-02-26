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
    bp::scope converter_scope = converter_module

// the minimal incantations to export pybindings for converters
// converter must be a legal python identifier (you can typedef this if necessary)

#define I3CONVERTER_EXPORT(converter)                              \
    bp::class_<converter,                                          \
               boost::shared_ptr<converter>,                       \
               bp::bases<I3Converter>,                             \
               boost::noncopyable >(BOOST_PP_STRINGIZE(converter))


#endif  