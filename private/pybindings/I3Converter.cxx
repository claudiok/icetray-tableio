/**
 * I3Converter.cxx (pybindings)
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


#include <hdf-writer/PythonConverter.h>

namespace bp = boost::python;

void register_I3Converter() {
	bp::class_<PythonConverter, boost::noncopyable>
		("I3Converter")
	
	.add_property("description",
		(I3TableRowDescriptionConstPtr (PythonConverter::*)()) &PythonConverter::GetDescription)
	.def("CreateDescription",&PythonConverter::CreateDescription)
	.def("Convert",(unsigned int (PythonConverter::*)(I3FrameObjectConstPtr, 
	                                                  I3TableRowPtr, 
	                                                  I3FrameConstPtr))
	               &PythonConverter::Convert)
	;
}