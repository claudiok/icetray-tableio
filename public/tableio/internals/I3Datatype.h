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

#ifndef I3DATATYPE_H_ZO84X7OI
#define I3DATATYPE_H_ZO84X7OI

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <vector>
#include <map>
#include <string>
#include <cctype>

// represents an atomic datatype
struct I3Datatype {
    enum TypeClass {
        Float = 0,
        Int   = 1,
        Enum  = 2,
        Bool  = 3  
    };
    TypeClass kind;
    size_t size;
    bool is_signed;
    std::vector<std::pair<std::string,long> > enum_members;
    const char* description;
    
    bool operator!=(const I3Datatype& rhs) const {
          return !(*this == rhs);
    };
    bool operator==(const I3Datatype& rhs) const {
          return ((kind == rhs.kind) &&
                  (size == rhs.size) &&
                  (is_signed == rhs.is_signed) &&
                  (enum_members == rhs.enum_members));
    };
    I3Datatype() {};
    I3Datatype(TypeClass k, size_t s, bool sign) : kind(k),size(s),is_signed(sign) {};
};

#define I3DATATYPE_FROM_NATIVE_TYPE(t) I3DatatypeFromNativeType_impl<t>(#t)
#define I3DATATYPE_FROM_ENUM_TYPE(t, enum_members) I3DatatypeFromNativeType_impl<t>(#t, enum_members)

template <typename T>
I3Datatype I3DatatypeFromNativeType_impl(const char* label) {
    BOOST_STATIC_ASSERT(boost::is_pod<T>::value);

    I3Datatype dtype;
    dtype.size = sizeof(T);
    
    if (boost::is_integral<T>()) {
        dtype.kind = I3Datatype::Int;
        dtype.is_signed = boost::is_signed<T>();
    } else { // it's a float
        dtype.kind = I3Datatype::Float;
        dtype.is_signed = true;
    }
    dtype.description = label;
    return dtype;
};

template <>
I3Datatype I3DatatypeFromNativeType_impl<bool>(const char* label);

template <typename T>
static I3Datatype I3DatatypeFromNativeType_impl(const char* label, 
                  const std::vector<std::pair< std::string, T> >& enum_labels) {
    BOOST_STATIC_ASSERT(boost::is_enum<T>::value);

    I3Datatype dtype;
    dtype.size = sizeof(T);
    dtype.kind = I3Datatype::Enum;
    dtype.is_signed = true; // some dataclasses use signed values in enums
   
    dtype.description = label;
    // copy the enum members into the map
    typename std::vector<std::pair<std::string,T> >::const_iterator it;
    for (it = enum_labels.begin(); it != enum_labels.end(); it++) {
        dtype.enum_members.push_back(std::make_pair(it->first,long(it->second)));
    }
    return dtype;
};

#endif
