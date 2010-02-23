/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include <tableio/internals/I3TableRow.h>
#include <dataclasses/I3Vector.h>
#include "type_helpers.h"

 namespace bp = boost::python;

/****************************************************************************/

 struct set_scalar {
     I3TableRow& self;
     size_t index;
     bp::object value;
     bool all;
     bool success;
     set_scalar(I3TableRow& s,size_t i,bp::object v,bool a) 
         : self(s), index(i), value(v), all(a), success(true) {};

     template <typename T>
     void call() {
         T v = bp::extract<T>(value);
         self.Set(index,v,all);
     };

     void fail(std::string message) {
         success = false;
         log_fatal("Badness: %s",message.c_str());
     };

 };
 
 /****************************************************************************/

 struct set_vector {
     I3TableRow& self;
     size_t index;
     bp::object value;
     bool all;
     bool success;
     set_vector(I3TableRow& s,size_t i,bp::object v,bool a) 
         : self(s), index(i), value(v), all(a), success(true) {};
     template <typename T>
     void call() {
         // T v = bp::extract<T>(value);
         std::vector<T> v = bp::extract<I3Vector<T> >(value);
         I3TableRowDescriptionConstPtr desc = self.GetDescription();
         // self.Set(index,v,all);
         if ( v.size() > desc->GetFieldArrayLengths().at(index) )
            log_fatal("Length mismatch between vector (%zu) and array field (%zu).",v.size(),desc->GetFieldArrayLengths().at(index));
         T* block = self.GetPointer<T>(index);
         std::copy(v.begin(),v.end(),block);
     };

     void fail(std::string message) {
         success = false;
         log_fatal("Badness: %s",message.c_str());
     };

 };
 
 /****************************************************************************/

 template<typename Dispatcher>
 bool set_index(I3TableRow& self, I3Datatype& dtype, size_t index, bp::object value, bool all) {
     Dispatcher dispatcher;
     typename Dispatcher::visitor_type visitor(self,index,value,all);
     dispatcher.route(dtype,visitor);
     return visitor.success;
 };
 
 /****************************************************************************/
 
 // =======================================================
 // = Copy the contents of a buffer into the memory chunk =
 // =======================================================
 bool try_set_ndarray(I3TableRow& self, I3Datatype dtype, size_t index, size_t array_length, bp::object value, bool all = false) {
     bool is_array = (value.attr("__class__").attr("__name__") == std::string("array"));
     bool is_ndarray = (value.attr("__class__").attr("__name__") == std::string("ndarray"));
     boost::shared_ptr<I3Datatype> arr_dtype;
     // const std::string field = self.GetDescription()->GetFieldNames().at(index);
     // ==============================================================
     // = Case 2.1: passed object is an array.array or numpy.ndarray =
     // ==============================================================
     if (is_array || is_ndarray) {
        if (is_array) {
           char arr_typecode = PyString_AsString(bp::object(value.attr("typecode")).ptr())[0];
           arr_dtype = I3Datatype_from_PyArrayTypecode(arr_typecode);
        } else {
           arr_dtype = I3Datatype_from_NumpyDtype(value.attr("dtype"));
        }
        // ==============================================
        // = Check the type and byte order of the array =
        // ==============================================
        if (!arr_dtype) log_fatal("Type of array could not be recognized.");
        if (*arr_dtype != dtype) log_fatal("Type of array does not match field");

        // hang on to your hats, here we go!
        const void* location;
        Py_ssize_t length;
        PyObject_AsReadBuffer(value.ptr(),&location,&length);
        if (length > 0) {
            // =======================================================
            // = Copy the buffer (an array in native representation) 
            //   directly into the memory chunk                      =
            // =======================================================
            if (static_cast<size_t>(length) > dtype.size*array_length)
               log_fatal("Size mismatch between memory block (%zu) and field (%zu)",
                          length,(dtype.size*array_length));
            size_t start,stop,i;
            void* pointy;
            if (all) {
                start = 0;
                stop = self.GetNumberOfRows();
            } else {
                start = self.GetCurrentRow();
                stop = start;
            }
            for (i = start; i < stop; i++) {
                pointy = self.GetPointerToField(index,i);
                memcpy(pointy,location,length);
            }
        }

        return true;
    } else {
        return false;
    }
 }
 
 /****************************************************************************/


// ==========================================================
// = Visitor that gets a scalar value from the memory chunk =
// ==========================================================
struct get_scalar {
    I3TableRow& self;
    size_t index;
    bp::object result;
    get_scalar(I3TableRow& s,size_t i) : self(s), index(i) {};
    
    template <typename T>
    void call() {
        result = bp::object(self.Get<T>(index));
    };
    
    void fail(std::string message) {
        log_fatal("Badness: %s",message.c_str());
    };

};

 /****************************************************************************/

// ===========================================================
// = Visitor that fills contents of memory chunk into a list =
// ===========================================================
// FIXME: we should return an array of some sort instead
// how do you construct an array.array from Python/C or boost::python?
struct get_list {
    I3TableRow& self;
    size_t index;
    bp::object result;
    get_list(I3TableRow& s,size_t i) : self(s), index(i) {};
    
    template <typename T>
    void call() {
        bp::list l;
        I3TableRowDescriptionConstPtr desc = self.GetDescription();
        size_t length = desc->GetFieldArrayLengths().at(index);
        T* block = self.GetPointer<T>(index);
        for (size_t i=0; i<length; i++) l.append(block[i]);
        result = l;
    };
    
    void fail(std::string message) {
        log_fatal("Badness: %s",message.c_str());
    };

};

template<typename T>
bp::object get_object(I3TableRow& self,I3Datatype& dtype,size_t index) {
    I3DatatypeDispatcher<T> dispatcher;
    T visitor(self,index);
    dispatcher.route(dtype,visitor);
    return visitor.result;
}
