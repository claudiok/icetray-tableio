/**
 * I3TableWriter.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include <hdf-writer/internals/I3TableWriter.h>

namespace bp = boost::python;

void register_I3TableWriter() {
	bp::scope tw = bp::class_<I3TableWriter, boost::noncopyable>
		("I3TableWriter",
		bp::init<I3TableServicePtr>(bp::args("table_service")))
		.def("add_object", (void (I3TableWriter::*)(const std::string, I3TableWriter::TableSpec)) &I3TableWriter::AddObject)
		.def("add_type", (void (I3TableWriter::*)(const std::string, I3TableWriter::TableSpec)) &I3TableWriter::AddType)
		.def("convert", (void (I3TableWriter::*)(I3FramePtr)) &I3TableWriter::Convert)
		.def("finish", &I3TableWriter::Finish)
	;
	
	bp::class_<I3TableWriter::TableSpec>("TableSpec")
		.def(bp::init<const std::string,I3ConverterPtr>())
		.def(bp::init<const std::string>())
		.def(bp::init<I3ConverterPtr>())
		;
}
