/**
 * I3TableRow.cxx (pybindings)
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

#include <tableio/internals/I3TableRow.h>
#include <dataclasses/I3Vector.h>

namespace bp = boost::python;

// ==============================================
// = Copy vector contents into the memory chunk =
// ==============================================
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

// =======================================================
// = Copy the contents of a buffer into the memory chunk =
// =======================================================
template <typename T>
void set_from_address(I3TableRow& self, const std::string& field, const void* location, size_t length, bool all = false) {
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   size_t size = sizeof(T);
   size_t array_length = desc->GetFieldArrayLengths().at(desc->GetFieldColumn(field));
   if (length > size*array_length)
      log_fatal("Size mismatch between memory block (%d) and field (%d) for '%s'",(int)length,(int)(size*array_length),field.c_str());
   T* block = self.GetPointer<T>(field);
   memcpy(block,location,length);
}

// =============================
// = Helpful templating macros =
// ============================= 
#define EXTRACT_SCALAR_SET(code,type) \
(type_code == code) { type val = bp::extract<type>(value); self.Set(field,val,all); success = true; }
#define EXTRACT_BUFFER(code,type) \
(type_code == code) { set_from_address<type>(self,field,location,length,all); success = true; }
#define EXTRACT_SCALAR(code,type) \
(type_code == code) { return get_object<type>(self,field); }
#define EXTRACT_LIST(code,type) \
(type_code == code) { return get_object_array<type>(self,field); }

// ========================================================
// = A python-friendly interface to I3TableRowSet<type>() =
// ========================================================
void set_field(I3TableRow& self, const std::string& field, bp::object value, bool all = false) {
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   int index = desc->GetFieldColumn(field);
   if (index < 0) log_fatal("Tried to set value for unknown column '%s'",field.c_str());
   
   char type_code = desc->GetFieldTypes().at(index).PythonTypeCode();
   bool success = false;
   // =====================================
   // = Case 1: field holds a scalar type =
   // =====================================
   if (desc->GetFieldArrayLengths().at(index) == 1) {
       if (type_code != 0) {
         if      EXTRACT_SCALAR_SET('c',char)
         else if EXTRACT_SCALAR_SET('b',signed char)
         else if EXTRACT_SCALAR_SET('B',unsigned char)
         else if EXTRACT_SCALAR_SET('h',signed short)
         else if EXTRACT_SCALAR_SET('H',unsigned short)
         else if EXTRACT_SCALAR_SET('i',signed int)
         else if EXTRACT_SCALAR_SET('I',unsigned int)
         else if EXTRACT_SCALAR_SET('l',signed long)
         else if EXTRACT_SCALAR_SET('L',unsigned long)
         else if EXTRACT_SCALAR_SET('f',float)
         else if EXTRACT_SCALAR_SET('d',double)
         else if EXTRACT_SCALAR_SET('o',bool)
         else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
      }
   // =====================================
   // = Case 2: field holds a vector type =
   // =====================================
   } else { // handle vectors
      bool is_array = (value.attr("__class__").attr("__name__") == std::string("array"));
      bool is_ndarray = (value.attr("__class__").attr("__name__") == std::string("ndarray"));
      // ==============================================================
      // = Case 2.1: passed object is an array.array or numpy.ndarray =
      // ==============================================================
      if (is_array || is_ndarray) {
         char arr_typecode = 0x20;
         char arr_byteorder = 0x20;
         if (is_array) {
            arr_typecode = PyString_AsString(bp::object(value.attr("typecode")).ptr())[0];
            arr_byteorder = '='; // array is always native
         } else {
            arr_typecode = PyString_AsString(bp::object(value.attr("dtype").attr("char")).ptr())[0];
            arr_byteorder = PyString_AsString(bp::object(value.attr("dtype").attr("byteorder")).ptr())[0];
         }
         // ==============================================
         // = Check the type and byte order of the array =
         // ==============================================
         if (arr_typecode != type_code) 
            log_fatal("Type of array ('%c') does not match field '%s' ('%c')",arr_typecode,field.c_str(),type_code);
         if (!(arr_byteorder == '|') && !(arr_byteorder == '='))
            log_fatal("Array must be in native byte order (not '%c')",arr_byteorder);
         // hang on to your hats, here we go!
         const void* location;
         Py_ssize_t length;
         PyObject_AsReadBuffer(value.ptr(),&location,&length);
         // =======================================================
         // = Copy the buffer (an array in native representation) 
         //   directly into the memory chunk                      =
         // =======================================================
         if      EXTRACT_BUFFER('c',char)
         else if EXTRACT_BUFFER('b',signed char)
         else if EXTRACT_BUFFER('B',unsigned char)
         else if EXTRACT_BUFFER('h',signed short)
         else if EXTRACT_BUFFER('H',unsigned short)
         else if EXTRACT_BUFFER('i',signed int)
         else if EXTRACT_BUFFER('I',unsigned int)
         else if EXTRACT_BUFFER('l',signed long)
         else if EXTRACT_BUFFER('L',unsigned long)
         else if EXTRACT_BUFFER('f',float)
         else if EXTRACT_BUFFER('d',double)
         else if EXTRACT_BUFFER('o',bool)
         else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
      // =================================================
      // = Case 2.2: passed object is a wrapped I3Vector =
      // =================================================  
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
   }
   // =======================================================
   // = If nothing could handle the value, raise ValueError =
   // =======================================================
   if (!success) {
      std::ostringstream msg;
      std::string classname = bp::extract<std::string>(value.attr("__class__").attr("__name__"));
      msg << "I3TableRow.set() can't interpret Python type '" << classname << "'.";
      PyErr_SetString(PyExc_ValueError, msg.str().c_str());
      throw bp::error_already_set();
   }
}

BOOST_PYTHON_FUNCTION_OVERLOADS(set_overloads,set_field,3,4)



// ============================================
// = Get a scalar value from the memory chunk =
// ============================================
template <typename T>
bp::object get_object(I3TableRow& self, const std::string& field) {
   return bp::object(self.Get<T>(field));
}

// =============================================
// = Fill contents of memory chunk into a list =
// =============================================
// FIXME: we should return an array of some sort instead
// how do you construct an array.array from Python/C or boost::python?
template <typename T>
bp::object get_object_array(I3TableRow& self, const std::string& field) {
   bp::list l;
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   unsigned int index = desc->GetFieldColumn(field);
   unsigned int length = desc->GetFieldArrayLengths().at(index);
   T* block = self.GetPointer<T>(field);
   for (unsigned int i=0; i<length; i++) l.append(block[i]);
   return l;
}

// =========================================================
// = A python-friendly interface to I3TableRow.Get<type>() =
// =========================================================
bp::object getitem(I3TableRow& self, const std::string& field) {
   
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   unsigned int index = desc->GetFieldColumn(field);
   if (index < 0) log_fatal("Tried to get value for unknown column '%s'",field.c_str());
   
   char type_code = desc->GetFieldTypes().at(index).PythonTypeCode();
   
   // =======================================
   // = Case 1: memory chunk holds a scalar =
   // =======================================
   if (desc->GetFieldArrayLengths().at(index) == 1) { // handle scalars
         if      EXTRACT_SCALAR('c',char)
         else if EXTRACT_SCALAR('b',signed char)
         else if EXTRACT_SCALAR('B',unsigned char)
         else if EXTRACT_SCALAR('h',signed short)
         else if EXTRACT_SCALAR('H',unsigned short)
         else if EXTRACT_SCALAR('i',signed int)
         else if EXTRACT_SCALAR('I',unsigned int)
         else if EXTRACT_SCALAR('l',signed long)
         else if EXTRACT_SCALAR('L',unsigned long)
         else if EXTRACT_SCALAR('f',float)
         else if EXTRACT_SCALAR('d',double)
         else if EXTRACT_SCALAR('o',bool)
         else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
   // =======================================
   // = Case 2: memory chunk holds a vector =
   // =======================================
   } else {
         if      EXTRACT_LIST('c',char)
         else if EXTRACT_LIST('b',signed char)
         else if EXTRACT_LIST('B',unsigned char)
         else if EXTRACT_LIST('h',signed short)
         else if EXTRACT_LIST('H',unsigned short)
         else if EXTRACT_LIST('i',signed int)
         else if EXTRACT_LIST('I',unsigned int)
         else if EXTRACT_LIST('l',signed long)
         else if EXTRACT_LIST('L',unsigned long)
         else if EXTRACT_LIST('f',float)
         else if EXTRACT_LIST('d',double)
         else if EXTRACT_LIST('o',bool)
         else log_fatal("Couldn\'t interpret type code '%c' ",type_code);
   }
}

// ======================
// = Get a list of keys =
// ======================
bp::list keys(I3TableRow& self) {
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   const std::vector<std::string>& fields = desc->GetFieldNames();
   std::vector<std::string>::const_iterator it;
   bp::list l;
   for (it=fields.begin();it!=fields.end();it++) {
      l.append(*it);
   }
   return l;
}

// =========================================================================
// = Initialize I3TableRow with an unsigned int, which Python doesn't have =
// =========================================================================
static boost::shared_ptr<I3TableRow> int_init( I3TableRowDescriptionPtr desc, int num_rows) {
   if (num_rows < 0) {
      PyErr_SetString(PyExc_ValueError, "num_rows must be >= 0");
      throw bp::error_already_set();
   } 
   return boost::shared_ptr<I3TableRow>(new I3TableRow(desc,static_cast<unsigned int>(num_rows)));
} 

// ================================
// = set_field with 'all = false' =
// ================================
void setitem(I3TableRow& self, const std::string& field, bp::object value) {
   set_field(self,field,value);
}


void register_I3TableRow() {
      
   bp::class_<I3TableRow,
      boost::shared_ptr<I3TableRow> >
      ("I3TableRow",
"\n\
An I3TableRow is a container for generic tabular data, i.e. columns and      \n\
rows. The associated I3TableRowDescriptionObject describes the format of the \n\
columns. Each column has a name and a datatype. The following basic data     \n\
types are supported:                                                         \n\
      ________________________________________________                       \n\
     | Code | Native type    | Equivalent Python type |                      \n\
     |------|----------------|------------------------|                      \n\
     |  'c' | char           |                        |                      \n\
     |  'b' | signed char    |                        |                      \n\
     |  'B' | unsigned char  |                        |                      \n\
     |  'h' | signed short   |                        |                      \n\
     |  'H' | unsigned short |                        |                      \n\
     |  'i' | signed int     | int                    |                      \n\
     |  'I' | unsigned int   |                        |                      \n\
     |  'l' | signed long    | long                   |                      \n\
     |  'L' | unsigned long  |                        |                      \n\
     |  'f' | float          |                        |                      \n\
     |  'd' | double         | float                  |                      \n\
     |  'o' | bool           | bool                   |                      \n\
      ------------------------------------------------                       \n\
                                                                             \n\
Data can be filled into or extracted from the rows of the object using the   \n\
standard dictionary notation:                                                \n\
                                                                             \n\
   desc = I3TableRowDescription()                                            \n\
   desc.add_field('foo','i','radians','The cut of foo\'s jib.')              \n\
   rows = I3TableRow(desc,5)                                                 \n\
                                                                             \n\
   for i in xrange(rows.number_of_rows)                                      \n\
      rows.current_row = i                                                   \n\
      rows['foo'] = i*5                                                      \n\
                                                                             \n\
Columns may also hold fixed-length vectors of homogenous data. If the        \n\
I3TableRow object was created thusly:                                        \n\
                                                                             \n\
   desc = I3TableRowDescription()                                            \n\
   desc.add_field('raw_waveform','H','counts','Digitizer counts',128)        \n\
   rows = I3TableRow(desc,1)                                                 \n\
                                                                             \n\
then the column 'raw_waveform' can be filled with a wrapped instance of      \n\
I3Vector                                                                     \n\
                                                                             \n\
   vec = dataclasses.I3VectorInt()                                           \n\
   for i in xrange(128): vec.append(i)                                       \n\
   rows['raw_waveform'] = vec                                                \n\
                                                                             \n\
or an array from the standard library array module                           \n\
                                                                             \n\
   import array                                                              \n\
   arr = array.array('H',range(128))                                         \n\
   rows['raw_waveform'] = arr                                                \n\
                                                                             \n\
or even a 1-d Numpy array                                                    \n\
                                                                             \n\
   import numpy                                                              \n\
   numpy_arr = numpy.array(range(128),'H')                                   \n\
   rows['raw_waveform'] = numpy_arr                                          \n\
                                                                             \n\
The datatype of the array must match the datatype of the column, or an error \n\
will be raised.                                                              \n\
",\
      bp::no_init)
   .def("__init__",bp::make_constructor(&int_init))
   #define PROPS (CurrentRow)(NumberOfRows)
   #define PROPS_RO (Description)
   BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRow,PROPS)
   BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRow,PROPS_RO)
   .def("set",set_field,set_overloads(bp::args("self","field","value","all")))
   .def("__setitem__",setitem)
   .def("__getitem__",getitem)
   .def("keys",keys)
   ;
}