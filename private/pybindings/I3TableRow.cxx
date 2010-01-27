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
	if ( value.size() != desc->GetFieldArrayLengths().at(desc->GetFieldColumn(field)) )
		log_fatal("Length mismatch between vector and array field.");
	// alright, copy the vector contents to the block
	T* block = self.GetPointer<T>(field);
	std::copy(value.begin(),value.end(),block);
}

// TODO: proxy function with type-switching magic
void set_field(I3TableRow& self, const std::string& field, bp::object value, bool all = false) {
	// check for Python core numeric types
	PyObject* ptr = value.ptr();
	bool success = false;
	if (PyBool_Check(ptr)) {
		bool val = (ptr == Py_True);
		success = true;
		self.Set(field,val,all);
	} else if (PyInt_Check(ptr)) {
		int val = bp::extract<int>(value);
		success = true;
		self.Set(field,val,all);
	} else if (PyFloat_Check(ptr)) {
		double val = PyFloat_AsDouble(ptr);
		success = true;
		self.Set(field,val,all);
	} else {
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
