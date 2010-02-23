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


#include <tableio/PythonConverter.h>

namespace bp = boost::python;

void register_I3Converter() {
	bp::class_<PythonConverter, boost::shared_ptr<PythonConverter>, boost::noncopyable>
		("I3Converter")
	
	.add_property("description",
		(I3TableRowDescriptionConstPtr (PythonConverter::*)()) &PythonConverter::GetDescription)
	.def("CreateDescription",(I3TableRowDescriptionPtr (PythonConverter::*)(I3FrameObjectConstPtr))
                    &PythonConverter::CreateDescription)
	.def("Convert",(size_t (PythonConverter::*)(I3FrameObjectConstPtr, 
	                                                  I3TableRowPtr, 
	                                                  I3FramePtr))
	               &PythonConverter::Convert)
	.def("CanConvert", (bool (PythonConverter::*)(I3FrameObjectPtr)) &PythonConverter::CanConvert)
	.def("FillRows",&PythonConverter::FillRows)
	.def("GetNumberOfRows",&PythonConverter::GetNumberOfRows)
	;
}