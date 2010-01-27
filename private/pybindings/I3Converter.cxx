#include <hdf-writer/PythonConverter.h>

namespace bp = boost::python;

// unsigned int (I3Converter::*convert)(
//                      const I3FrameObject&, 
//                      I3TableRowPtr, 
//                      I3FrameConstPtr);

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