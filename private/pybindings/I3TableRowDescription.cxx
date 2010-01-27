#include <hdf-writer/internals/I3TableRowDescription.h>

// using namespace boost::python;
namespace bp = boost::python;


// A python-friendly interface to I3TableRowDescription
void add_field(I3TableRowDescription& self, const std::string& name, bp::object py_type, const std::string& unit, const std::string& doc, size_t array_length = 1) {
	bp::object base = py_type.attr("__base__").attr("__name__");
	std::string basename = bp::extract<std::string>(base);
	// log_info("basename: %s\n",basename.c_str());
	typedef std::pair<std::string,int> pair;
	if (basename == std::string("enum")) {
		// extract the entries in the wrapped
		// Boost.Python.enum
		log_info("handling enum");
		std::vector<pair> values;
		bp::tuple entry;
		std::string entry_name;
		int entry_value;
		bp::list dict_entries(py_type.attr("values").attr("items")());
		bp::ssize_t n = bp::len(dict_entries);
		for (int i=0; i<n; i++) {
			// TODO: check results
			entry = bp::extract<bp::tuple>(dict_entries[i]);
			entry_name = bp::extract<std::string>(entry[1].attr("name"));
			entry_value = bp::extract<int>(entry[0]);
			values.push_back(pair(entry_name,entry_value));
		}
		self.AddEnumField(name,values,unit,doc,array_length);
		
	} else {
		// handle all other types
		// TODO: can anything else concievably be passed through pybindings?
		// the union mechanism doesn't save us any space anyhow...
		std::string classname = bp::extract<std::string>(py_type.attr("__name__"));
		log_info("handling '%s'",classname.c_str());
		if (classname == std::string("float")) {
			self.AddField<double>(name,unit,doc,array_length);
		} else if (classname == std::string("int")) {
			self.AddField<int>(name,unit,doc,array_length);
		} else if (classname == std::string("bool")) {
			self.AddField<bool>(name,unit,doc,array_length);
		} else {
			std::ostringstream msg;
			msg << "I3TableRowDescription can't interpret Python type '" << classname << "'.";
			PyErr_SetString(PyExc_ValueError, msg.str().c_str());
			throw bp::error_already_set();
		}
	}
	
}
BOOST_PYTHON_FUNCTION_OVERLOADS(add_field_overloads,add_field,5,6)

void register_I3TableRowDescription() {


	bp::class_<I3TableRowDescription, 
		boost::shared_ptr<I3TableRowDescription> >
		("I3TableRowDescription")
		#define RO_VEC_PROPERTIES (FieldNames)(FieldHdfTypes)(FieldTypeSizes)(FieldByteOffsets)(FieldChunkOffsets)(FieldArrayLengths)(FieldUnits)(FieldDocStrings)
		#define RO_PROPERTIES (TotalByteSize)(TotalChunkSize)(NumberOfFields)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO_INTERNAL_REFERENCE,I3TableRowDescription,RO_VEC_PROPERTIES)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRowDescription,RO_PROPERTIES)
		.def("add_field",add_field,
			add_field_overloads(
				bp::args("name","python_type","units","docstring","array_size"),
				"Add a field of type based on python_type to the table description. \
				Boost.Python.enums will be translated into HDF5 enums."
				))
	;
}