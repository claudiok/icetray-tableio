#include <hdf-writer/PythonConverter.h>
#include <boost/python.hpp>

namespace bp = boost::python;

PythonConverter::PythonConverter() : I3Converter() {
	log_trace("%s",__PRETTY_FUNCTION__);
}


unsigned int PythonConverter::GetNumberOfRows(I3FrameObjectConstPtr object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override num_rows = this->get_override("GetNumberOfRows")) {
		return num_rows(boost::const_pointer_cast<I3FrameObject>(object));
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
		I3TableRowDescriptionPtr desc_ptr = create_desc(object);
		if (!desc_ptr) {
			log_fatal("CreateDescription(frame_object) must return an I3TableRowDescription.");
		} else {
			return desc_ptr;
		}
	} else {
		log_fatal("Python module must implement CreateDescription(frame_object).");
	}
}

unsigned int PythonConverter::Convert(I3FrameObjectConstPtr object, 
                             I3TableRowPtr rows, 
                             I3FramePtr frame) {
	log_trace("%s",__PRETTY_FUNCTION__);
	// return Convert(*object,rows,frame);
	// unsigned int nrows;
	if (bp::override fillrows = this->get_override("FillRows")) {
		return fillrows(boost::const_pointer_cast<I3FrameObject>(object),rows);
	} else if (bp::override convert = this->get_override("Convert")) {
		return convert(boost::const_pointer_cast<I3FrameObject>(object),rows,frame);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame) or FillRows(frame_object,rows).");
	}
}

unsigned int PythonConverter::FillRows(const I3FrameObjectPtr object, I3TableRowPtr rows) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override fillrows = this->get_override("FillRows")) {
		return fillrows(boost::const_pointer_cast<I3FrameObject>(object),rows);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame) or FillRows(frame_object,rows).");
	}
}


unsigned int PythonConverter::Convert(const I3FrameObject& object, 
                             I3TableRowPtr rows, 
                             I3FramePtr frame) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override convert = this->get_override("Convert")) {
		return convert(object,rows,frame);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame).");
	}
}
