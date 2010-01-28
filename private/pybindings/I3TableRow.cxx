#include <hdf-writer/internals/I3TableRow.h>
#include <dataclasses/I3Vector.h>

// using namespace boost::python;
namespace bp = boost::python;

// fill a memory chunk with the vector contents
template <typename T>
void set_vector(I3TableRow& self, const std::string& field, std::vector<T> value, bool all = false) {
	// TODO: support 'all' parameter
	I3TableRowDescriptionConstPtr desc = self.GetDescription();
	// are we going to hose our memory?
	if ( value.size() > desc->GetFieldArrayLengths().at(desc->GetFieldColumn(field)) )
		log_fatal("Length mismatch between vector (%d) and array field (%d).",(int)value.size(),(int)desc->GetFieldArrayLengths().at(desc->GetFieldColumn(field)));
	// alright, copy the vector contents to the block
	T* block = self.GetPointer<T>(field);
	std::copy(value.begin(),value.end(),block);
}

template <typename T>
void set_from_address(I3TableRow& self, const std::string& field, void* location, size_t length, bool all = false) {
	I3TableRowDescriptionConstPtr desc = self.GetDescription();
	size_t size = sizeof(T);
	size_t array_length = desc->GetFieldArrayLengths().at(desc->GetFieldColumn(field));
	if (length > size*array_length)
		log_fatal("Size mismatch between memory block (%d) and field (%d) for '%s'",(int)length,(int)(size*array_length),field.c_str());
	T* block = self.GetPointer<T>(field);
	// is this union-safe?
	memcpy(block,location,length);
}

 
#define EXTRACT_SCALAR(code,type) \
(type_code == code) { type val = bp::extract<type>(value); self.Set(field,val,all); success = true; }

#define EXTRACT_BUFFER(code,type) \
(type_code == code) { set_from_address<type>(self,field,&location,length,all); success = true; }

// TODO: proxy function with type-switching magic
void set_field(I3TableRow& self, const std::string& field, bp::object value, bool all = false) {
	I3TableRowDescriptionConstPtr desc = self.GetDescription();
	int index = desc->GetFieldColumn(field);
	if (index < 0) log_fatal("Tried to set value for unknown column '%s'",field.c_str());
	
	// char type_code = PyTypeConv::GetTypeCode(desc->GetFieldHdfTypes().at(index));
	char type_code = desc->GetFieldTypeCodes().at(index);
	bool success = false;
	
	if (desc->GetFieldArrayLengths().at(index) == 1) { // handle scalars
		PyObject* ptr = value.ptr();
		if (PyBool_Check(ptr)) {
			bool val = (ptr == Py_True);
			success = true;
			self.Set(field,val,all);
		} else if (type_code != 0) {
			if      EXTRACT_SCALAR('c',char)
			else if EXTRACT_SCALAR('b',signed char)
			else if EXTRACT_SCALAR('B',unsigned char)
			else if EXTRACT_SCALAR('h',signed short)
			else if EXTRACT_SCALAR('H',unsigned short)
			else if EXTRACT_SCALAR('i',signed int)
			else if EXTRACT_SCALAR('I',unsigned int)
			else if EXTRACT_SCALAR('h',signed long)
			else if EXTRACT_SCALAR('H',unsigned long)
			else if EXTRACT_SCALAR('f',float)
			else if EXTRACT_SCALAR('d',double)
			else if EXTRACT_SCALAR('o',bool)
			else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
		}
	} else { // handle vectors
		bool is_array = (value.attr("__class__").attr("__name__") == std::string("array"));
		bool is_ndarray = (value.attr("__class__").attr("__name__") == std::string("ndarray"));
		if (is_array || is_ndarray) { // handle array.array and numpy.ndarray
			char arr_typecode = 0x20;
			char arr_byteorder = 0x20;
			if (is_array) {
				arr_typecode = PyString_AsString(bp::object(value.attr("typecode")).ptr())[0];
				arr_byteorder = '='; // array is always native
			} else {
				arr_typecode = PyString_AsString(bp::object(value.attr("dtype").attr("char")).ptr())[0];
				arr_byteorder = PyString_AsString(bp::object(value.attr("dtype").attr("byteorder")).ptr())[0];
			}
			if (arr_typecode != type_code) 
				log_fatal("Type of array ('%c') does not match field '%s' ('%c')",arr_typecode,field.c_str(),type_code);
			if (!(arr_byteorder == '|') && !(arr_byteorder == '='))
				log_fatal("Array must be in native byte order (not '%c')",arr_byteorder);
			// hang on to your hats, here we go!
			const void* location;
			Py_ssize_t length;
			PyObject_AsReadBuffer(value.ptr(),&location,&length);
			// PyBufferProcs* procs = value.ptr()->ob_type->tp_as_buffer;
			if      EXTRACT_BUFFER('c',char)
			else if EXTRACT_BUFFER('b',signed char)
			else if EXTRACT_BUFFER('B',unsigned char)
			else if EXTRACT_BUFFER('h',signed short)
			else if EXTRACT_BUFFER('H',unsigned short)
			else if EXTRACT_BUFFER('i',signed int)
			else if EXTRACT_BUFFER('I',unsigned int)
			else if EXTRACT_BUFFER('h',signed long)
			else if EXTRACT_BUFFER('H',unsigned long)
			else if EXTRACT_BUFFER('f',float)
			else if EXTRACT_BUFFER('d',double)
			else if EXTRACT_BUFFER('o',bool)
			else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
			
		} else { // handle a few special cases of I3Vector
			bp::extract<I3VectorInt> extract_int(value);
			bp::extract<I3VectorDouble> extract_double(value);
			if (extract_int.check()) {
				success = true;
				set_vector(self,field,static_cast<std::vector<int> >(extract_int()),all);
			} else if (extract_double.check()) {
				success = true;
				set_vector(self,field,static_cast<std::vector<double> >(extract_double()),all);
			}
		}
	}
	
	if (!success) {
		std::ostringstream msg;
		std::string classname = bp::extract<std::string>(value.attr("__class__").attr("__name__"));
		msg << "I3TableRow.set() can't interpret Python type '" << classname << "'.";
		PyErr_SetString(PyExc_ValueError, msg.str().c_str());
		throw bp::error_already_set();
	}
}

BOOST_PYTHON_FUNCTION_OVERLOADS(set_overloads,set_field,3,4)

static boost::shared_ptr<I3TableRow> int_init( I3TableRowDescriptionPtr desc, int num_rows) {
	if (num_rows < 0) {
		PyErr_SetString(PyExc_ValueError, "num_rows must be >= 0");
		throw bp::error_already_set();
	} 
	return boost::shared_ptr<I3TableRow>(new I3TableRow(desc,static_cast<unsigned int>(num_rows)));
} 


void register_I3TableRow() {
	
	bp::class_<I3TableRow,
		boost::shared_ptr<I3TableRow> >
		("I3TableRow",
		bp::no_init)
	.def("__init__",bp::make_constructor(&int_init))
	#define PROPS (CurrentRow)(NumberOfRows)
	#define PROPS_RO (Description)
	BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRow,PROPS)
	BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRow,PROPS_RO)
	.def("set",set_field,set_overloads(bp::args("self","field","value","all")))
	;
}
