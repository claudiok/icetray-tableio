/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3_TABLEROWDESCRIPTION_H_INCLUDED
#define I3_TABLEROWDESCRIPTION_H_INCLUDED

// icetray includes
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "tableio/internals/I3Datatype.h"

#include <string>
#include <vector>

// boost::assign for less tedious enums
#include <boost/assign/list_inserter.hpp>
#include <boost/preprocessor/seq/transform.hpp>

#include "tableio/internals/I3MemoryChunk.h"

/*****************************************************************************/

#define WRAP_ELEMENTS(R,DATA,ELEM) BOOST_PP_STRINGIZE(ELEM),DATA::ELEM
#define MAKE_ENUM_VECTOR(VECTORNAME,CLASS,ENUM_TYPE,FIELDS)		\
    std::vector< std::pair<std::string,CLASS::ENUM_TYPE> > VECTORNAME ;	\
    boost::assign::push_back(VECTORNAME) BOOST_PP_SEQ_TRANSFORM(WRAP_ELEMENTS,CLASS,FIELDS); \

/*****************************************************************************/

/*****************************************************************************/


class I3TableRowDescription {


public:
    I3TableRowDescription();
    virtual ~I3TableRowDescription();

    /* basic AddField */
    void AddField(const std::string& name, I3Datatype type,
		  size_t typeSize, const std::string& unit,
		  const std::string& doc,
		  size_t arrayLength);

    /* convenience AddField */
    template<class T>
    void AddField(const std::string& name, 
		  const std::string& unit,
		  const std::string& doc, 
		  size_t arrayLength=1) 
    {
	AddField(name, I3DatatypeFromNativeType(T), sizeof(T), 
		 unit, doc, arrayLength);
    }

    /* convenience AddEnumField - create hdf type and call AddField */
    template<typename enum_type>
    void AddEnumField(const std::string& name, 
		      const std::vector<std::pair<std::string,enum_type> > &elements,
		      const std::string& unit,
		      const std::string& doc,
		      size_t arrayLength=1) {
            
	// TODO check enumsize <= DATAENTRYSIZE
    // typename std::vector<std::pair<std::string,enum_type> >::const_iterator it;
    // enum_type enum_instance;
    // hid_t enum_tid = H5Tcreate(H5T_ENUM, sizeof(enum_type));
    // for (it = elements.begin(); it != elements.end(); ++it) {
    //     H5Tenum_insert(enum_tid, it->first.c_str(),
    //         (enum_instance=it->second, &enum_instance));
    // }
    I3Datatype enum_typus = I3DatatypeFromEnumType(enum_type, elements);
    // char typeCode = 'i';
	AddField(name, enum_typus, sizeof(enum_type), unit, doc, arrayLength);
    }

    bool CanBeFilledInto(shared_ptr<const I3TableRowDescription> other) const;
        
    // getter and setter - remove them? no real encapsulation anyway
    const std::vector<std::string>& GetFieldNames() const;
    const std::vector<I3Datatype>&  GetFieldTypes() const;
    // const std::vector<char>& GetFieldTypeCodes() const;
    // units of bytes
    const std::vector<size_t>& GetFieldTypeSizes() const;
    const std::vector<size_t>& GetFieldByteOffsets() const;
    const std::vector<size_t>& GetFieldChunkOffsets() const;
    const std::vector<size_t>& GetFieldArrayLengths() const;
    const std::vector<std::string>& GetFieldUnits() const;
    const std::vector<std::string>& GetFieldDocStrings() const;

    unsigned int GetFieldColumn(const std::string& fieldName) const;
    size_t GetTotalByteSize() const;
    size_t GetTotalChunkSize() const;
    unsigned int GetNumberOfFields() const;

    // Will this description be used for an object flattened across multiple rows?
    bool isMultiRow_;
    bool GetIsMultiRow() const { return isMultiRow_; };
    void SetIsMultiRow(bool f) { isMultiRow_ = f; };
    

    bool operator==(shared_ptr<const I3TableRowDescription> other) const;
private:
    std::vector<std::string> fieldNames_;
    std::map<std::string, size_t> fieldNameToIndex_;
    std::vector<I3Datatype> fieldTypes_;
    // std::vector<char> fieldTypeCodes_;
    std::vector<size_t> fieldTypeSizes_;
    std::vector<size_t> fieldArrayLengths_;
    std::vector<size_t> fieldByteOffsets_;
    std::vector<size_t> fieldChunkOffsets_;
    std::vector<std::string> fieldUnits_;
    std::vector<std::string> fieldDocStrings_;


/*     
#define CODEMAP_DEF(TYPE, CODE, TYPECODE)                \
    const static inline char  py_code(TYPE)  { return TYPECODE; }
    // const static inline hid_t hdf_type(TYPE) { return  CODE; }       
    
    
    CODEMAP_DEF(float, H5T_NATIVE_FLOAT, 'f');
    CODEMAP_DEF(double, H5T_NATIVE_DOUBLE, 'd');
    CODEMAP_DEF(long double, H5T_NATIVE_LDOUBLE, '\0');
    CODEMAP_DEF(char, H5T_NATIVE_CHAR, 'c');
    CODEMAP_DEF(unsigned char, H5T_NATIVE_UCHAR, 'B');
    CODEMAP_DEF(signed char, H5T_NATIVE_SCHAR, 'b');
    CODEMAP_DEF(short, H5T_NATIVE_SHORT, 'h');
    CODEMAP_DEF(unsigned short, H5T_NATIVE_USHORT, 'H');
    CODEMAP_DEF(int, H5T_NATIVE_INT, 'i');
    CODEMAP_DEF(unsigned int, H5T_NATIVE_UINT, 'I');
    CODEMAP_DEF(long int, H5T_NATIVE_LONG, 'l');
    CODEMAP_DEF(unsigned long int, H5T_NATIVE_ULONG, 'L');
    CODEMAP_DEF(bool, H5T_NATIVE_CHAR, 'o'); // HBOOL is really an int

  #undef CODEMAP_DEF
*/

/*
const static inline char py_code_from_hdf(hid_t hdf_type) { 
   // grab the native datatype of the atom 
   if ( H5Tget_class(hdf_type) == H5T_ENUM ) { 
        hdf_type = H5Tget_super(hdf_type); 
   } 
   hid_t n = H5Tget_native_type(hdf_type,H5T_DIR_ASCEND); 
   char code = 0; 
   if        ( H5Tequal(n,H5T_NATIVE_FLOAT)  ) { 
          code = 'f'; // python double        
   } else if ( H5Tequal(n,H5T_NATIVE_DOUBLE) ) { 
          code = 'd'; // python double        
   } else if ( H5Tequal(n,H5T_NATIVE_CHAR)   ) { 
          code = 'c'; // python character     
   } else if ( H5Tequal(n,H5T_NATIVE_UCHAR)  ) { 
          code = 'B'; // python int           
   } else if ( H5Tequal(n,H5T_NATIVE_SCHAR)  ) { 
          code = 'b'; // python int           
   } else if ( H5Tequal(n,H5T_NATIVE_SHORT)  ) { 
          code = 'h'; // python int           
   } else if ( H5Tequal(n,H5T_NATIVE_USHORT) ) { 
          code = 'H'; // python int           
   } else if ( H5Tequal(n,H5T_NATIVE_INT)    ) { 
          code = 'i'; // python int           
   } else if ( H5Tequal(n,H5T_NATIVE_UINT)   ) { 
          code = 'I'; // python long          
   } else if ( H5Tequal(n,H5T_NATIVE_LONG)   ) { 
          code = 'l'; // python long          
   } else if ( H5Tequal(n,H5T_NATIVE_ULONG)  ) { 
          code = 'L'; // python long 
   } //else if ( H5Tequal(n,H5T_NATIVE_HBOOL)  ) { 
     //     code = 'o'; // bool 
   //} 
   H5Tclose(n); 
   return code; 
}
*/

    friend I3TableRowDescription operator|(const I3TableRowDescription& lhs, const I3TableRowDescription& rhs);

    SET_LOGGER("I3TableRowDescription");
};
    
// declare template specialization for bools to let other file find
// the implementation in the .cxx file
template<> void I3TableRowDescription::AddField<bool>(const std::string& name,
                                                      const std::string& unit,
                                                      const std::string& doc,
                                                      size_t arrayLength);

I3_POINTER_TYPEDEFS( I3TableRowDescription );

#endif
