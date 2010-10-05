/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_DATACLASSES_VECTOR_CONVERTERS_H_INCLUDED
#define TABLEIO_DATACLASSES_VECTOR_CONVERTERS_H_INCLUDED

#include "tableio/converter/I3VectorConverter.h"
#include <utility>
#include <dataclasses/physics/I3FlasherInfo.h>

namespace convert {

  namespace detail {

    template <typename T>
    struct pod_converter_type_mapping {
      typedef T type;
    };
    
    template <>
    struct pod_converter_type_mapping<signed short> {
      typedef int16_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<signed int> {
      typedef int32_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<signed long> {
      typedef int64_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<signed long long> {
      typedef int64_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<unsigned short> {
      typedef uint16_t type;
    };

    template <>
    struct pod_converter_type_mapping<unsigned int> {
      typedef uint32_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<unsigned long> {
      typedef uint64_t type;
    };
    
    template <>
    struct pod_converter_type_mapping<unsigned long long> {
      typedef uint64_t type;
    };
    
  }


  template <typename T>
  struct pod {
    typedef T value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type())
    {
      desc->AddField< typename detail::pod_converter_type_mapping<T>::type >("item", "", "Item in the booked vector");
    }

    void FillSingleRow(const value_type &item, I3TableRowPtr row)
    {
      row->Set< typename detail::pod_converter_type_mapping<T>::type >("item", item);
    }
  };


  struct double_pair {
    typedef std::pair<double, double> value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type &item, I3TableRowPtr row);
  };


  struct I3FlasherInfo {
    typedef ::I3FlasherInfo value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type &flasherinfo, I3TableRowPtr row);
  };

}

#endif // TABLEIO_DATACLASSES_VECTOR_CONVERTERS_H_INCLUDED
