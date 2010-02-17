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

#include "hdf-writer/internals/I3Converter.h"

namespace bp = boost::python;

// sets up a namespace "proj.converters" into which pybound converters
// can be exported

#define I3CONVERTER_NAMESPACE(proj)                                                                                    \
    bp::object converter_module(bp::handle<>(bp::borrowed(PyImport_AddModule(BOOST_PP_STRINGIZE(proj)".converters"))));\
    bp::scope().attr("converters") = converter_module;                                                                 \
    bp::scope converter_scope = converter_module

// the minimal incantations to export pybindings for converters

#define I3CONVERTER_EXPORT(converter)                              \
    bp::class_<converter,                                          \
               boost::shared_ptr<converter>,                       \
               bp::bases<I3Converter>,                             \
               boost::noncopyable >(BOOST_PP_STRINGIZE(converter))


#endif  