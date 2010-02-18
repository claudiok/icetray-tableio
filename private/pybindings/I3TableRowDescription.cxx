/**
 * I3TableRowDescription.cxx (pybindings)
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

#include <tableio/internals/I3TableRowDescription.h>

namespace bp = boost::python;

// ========================================================
// = A python-friendly interface to I3TableRowDescription =
// ========================================================
void add_field(I3TableRowDescription& self, const std::string& name, bp::object py_type, const std::string& unit, const std::string& doc, size_t array_length = 1) {

   bool success = false;
   // =====================================
   // = Case 1: user passed a Python type =
   // =====================================
   if ( !PyString_Check(py_type.ptr()) ) {
      bp::object base = py_type.attr("__base__").attr("__name__");
      std::string basename = bp::extract<std::string>(base);
      typedef std::pair<std::string,int> pair;
      // ========================================================
      // = Case 1.1: user passed a Boost.Python.enum
      //   Extract the entries and construct an enumerated type =
      // ========================================================
      if (basename == std::string("enum")) {
         log_trace("handling enum");
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
         success = true;
         self.AddEnumField(name,values,unit,doc,array_length);
      // =============================================
      // = Case 1.2: user passed a core numeric type =
      // =============================================
      } else {
         std::string classname = bp::extract<std::string>(py_type.attr("__name__"));
         log_trace("handling '%s'",classname.c_str());
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
   // ===================================
   // = Case 2: user passed a type code =
   // ===================================
   } else {
      if (PyString_Size(py_type.ptr()) != 1) {
         PyErr_SetString(PyExc_ValueError, "Type code must be a single character.");
         throw bp::error_already_set();
      }
      char code = PyString_AsString(py_type.ptr())[0];
      success = true;
      switch (code) {
         case 'f':
            self.AddField<float>(name,unit,doc,array_length);
            break;
         case 'd':
            self.AddField<double>(name,unit,doc,array_length);
            break;
         case 'i':
            self.AddField<int>(name,unit,doc,array_length);
            break;
         case 'I':
            self.AddField<unsigned int>(name,unit,doc,array_length);
            break;
         case 'c':
            self.AddField<char>(name,unit,doc,array_length);
            break;
         case 'b':
            self.AddField<signed char>(name,unit,doc,array_length);
            break;
         case 'B':
            self.AddField<unsigned char>(name,unit,doc,array_length);
            break;
         case 'h':
            self.AddField<signed short>(name,unit,doc,array_length);
            break;
         case 'H':
            self.AddField<unsigned short>(name,unit,doc,array_length);
            break;
         case 'l':
            self.AddField<signed long>(name,unit,doc,array_length);
            break;
         case 'L':
            self.AddField<unsigned long>(name,unit,doc,array_length);
            break;
         case 'o':
            self.AddField<bool>(name,unit,doc,array_length);
            break;
         default:
            success = false;
      }
      if (!success) {
         std::ostringstream msg;
         msg << "I3TableRowDescription can't interpret type code '" << code << "'.";
         PyErr_SetString(PyExc_ValueError, "Type code must be a single character.");
         throw bp::error_already_set();
      }
   }
   
}
BOOST_PYTHON_FUNCTION_OVERLOADS(add_field_overloads,add_field,5,6)

void register_I3TableRowDescription() {


   bp::class_<I3TableRowDescription, 
      boost::shared_ptr<I3TableRowDescription>,
      boost::shared_ptr<I3TableRowDescription const> >
      ("I3TableRowDescription")
      #define RO_VEC_PROPERTIES (FieldNames)(FieldTypes)(FieldTypeSizes)(FieldByteOffsets)(FieldChunkOffsets)(FieldArrayLengths)(FieldUnits)(FieldDocStrings)
      #define RO_PROPERTIES (TotalByteSize)(TotalChunkSize)(NumberOfFields)
      #define PROPERTIES (IsMultiRow)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO_INTERNAL_REFERENCE,I3TableRowDescription,RO_VEC_PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRowDescription,RO_PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRowDescription,PROPERTIES)
      
      .def("add_field",add_field,
         add_field_overloads(
            bp::args("name","dtype","units","docstring","array_size"),
"\nAdd a field to the table description.                                      \n\
                                                                              \n\
Parameters                                                                    \n\
----------                                                                    \n\
name : string                                                                 \n\
   The name of the field. This should be a valid Python identifier.           \n\
dtype : character, type, or Boost.Python.enum                                 \n\
   The datatype to be stored in the field. The following single-character     \n\
   codes can be used to specify native datatypes:                             \n\
      ________________________________________________                        \n\
     | Code | Native type    | Equivalent Python type |                       \n\
     |------|----------------|------------------------|                       \n\
     |  'c' | char           |                        |                       \n\
     |  'b' | signed char    |                        |                       \n\
     |  'B' | unsigned char  |                        |                       \n\
     |  'h' | signed short   |                        |                       \n\
     |  'H' | unsigned short |                        |                       \n\
     |  'i' | signed int     | int                    |                       \n\
     |  'I' | unsigned int   |                        |                       \n\
     |  'l' | signed long    | long                   |                       \n\
     |  'L' | unsigned long  |                        |                       \n\
     |  'f' | float          |                        |                       \n\
     |  'd' | double         | float                  |                       \n\
     |  'o' | bool           | bool                   |                       \n\
      ------------------------------------------------                        \n\
    These are the same codes used in the array and struct modules.            \n\
    Alternatively, you may pass int, float, long, or bool to specify the      \n\
    datatype. If you pass an instance of Boost.Python.enum, the names of the  \n\
    enum members will be extracted and stored in the final table; the enum    \n\
    itself will be represented as an int.                                     \n\
units : string                                                                \n\
   The units of the values stored in the field (e.g. 'mV', 'm', 'Gm/s')       \n\
docstring : string                                                            \n\
   A description of quantity stored in the field (e.g. where it comes from and\n\
   how it was calculated).                                                    \n\
array_size : int                                                              \n\
   The number of elements in the field. If this is 1 (default), then the field\n\
   is a scalar. Otherwise, the field will store a fixed-length vector         \n\
   quantity.                                                                  \n\
                                                                              \n\
Returns                                                                       \n\
-------                                                                       \n\
None                                                                          \n\
"))

   ;
}