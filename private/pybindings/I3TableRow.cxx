#include <hdf-writer/internals/I3TableRow.h>

// using namespace boost::python;
namespace bp = boost::python;

// TODO: proxy function with type-switching magic
void set(I3TableRow& self, const std::string& field, bp::object value, bool all = false) {
	// check for Python core numeric types
	PyObject* ptr = value.ptr();
	if (PyBool_Check(ptr)) {
		bool val = (ptr == Py_True);
		self.Set(field,val,all);
	} else if (PyInt_Check(ptr)) {
		int val = bp::extract<int>(value);
		self.Set(field,val,all);
	} else if (PyFloat_Check(ptr)) {
		double val = PyFloat_AsDouble(ptr);
		self.Set(field,val,all);
	} else {
		std::ostringstream msg;
		std::string classname = bp::extract<std::string>(value.attr("__class__").attr("__name__"));
		msg << "I3TableRow.set() can't interpret Python type '" << classname << "'.";
		PyErr_SetString(PyExc_ValueError, msg.str().c_str());
		throw bp::error_already_set();
	}
}

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
	.def("set",set)
	;
}
