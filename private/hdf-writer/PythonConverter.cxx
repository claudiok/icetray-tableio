#include <hdf-writer/PythonConverter.h>
#include <boost/python.hpp>

namespace bp = boost::python;

PythonConverter::PythonConverter() : I3Converter() {
	log_trace("%s",__PRETTY_FUNCTION__);
}


unsigned int PythonConverter::GetNumberOfRows(I3FrameObjectConstPtr object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override num_rows = this->get_override("GetNumberOfRows")) {
		return num_rows(object);
	} else {
		return 1;
	}
}


I3TableRowDescriptionConstPtr PythonConverter::GetDescription(I3FrameObjectConstPtr object) {
	return GetDescription(*object);
}

I3TableRowDescriptionConstPtr PythonConverter::GetDescription(const I3FrameObject& object) {
	log_trace("%s",__PRETTY_FUNCTION__);    
	if (description_)
	        return description_;
	    else {
	        description_ = CreateDescription(object); 
	        return description_;
	    }
}

I3TableRowDescriptionConstPtr PythonConverter::GetDescription() {
	log_trace("%s",__PRETTY_FUNCTION__);    
	if (description_)
	        return description_;
	    else {
		log_fatal("No description set.");
	    }
}

I3TableRowDescriptionPtr PythonConverter::CreateDescription(const I3FrameObject& object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override create_desc = this->get_override("CreateDescription")) {
		return create_desc(object);
	} else {
		log_fatal("Python module must implement CreateDescription(frame_object).");
	}
}

unsigned int PythonConverter::Convert(I3FrameObjectConstPtr object, 
                             I3TableRowPtr rows, 
                             I3FrameConstPtr frame) {
	return Convert(*object,rows,frame);
}

unsigned int PythonConverter::Convert(const I3FrameObject& object, 
                             I3TableRowPtr rows, 
                             I3FrameConstPtr frame) {
				log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override convert = this->get_override("Convert")) {
		return convert(object);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame).");
	}
}