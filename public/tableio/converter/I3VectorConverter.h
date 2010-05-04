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

#ifndef TABLEIO_I3VECTORCONVERTER_H_INCLUDED
#define TABLEIO_I3VECTORCONVERTER_H_INCLUDED

#include <tableio/internals/I3ConverterFactory.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3RecoPulse.h>


namespace vector_converter {

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


  template <typename T>
  struct convert {
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField< typename pod_converter_type_mapping<T>::type >("item", "", "Item in the booked vector");
    }

    static void FillRow(const T &item, I3TableRowPtr row)
    {
      row->Set< typename pod_converter_type_mapping<T>::type >("item", item);
    }
  };


  template <>
  struct convert< std::pair<double, double> > {
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField< double >("first", "", "Vector of double-double pair: first item");
      desc->AddField< double >("second", "", "Vector of double-double pair: second item");
    }

    static void FillRow(const std::pair<double, double> &item, I3TableRowPtr row)
    {
      row->Set< double >("first", item.first);
      row->Set< double >("second", item.second);
    }
  };

}


template <class T,
	  typename converter_type = typename vector_converter::convert<T> >
class I3VectorConverter : public I3ConverterImplementation< I3Vector<T> > {
private:
  size_t GetNumberOfRows(const I3Vector<T> &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    return v.size();
  }

  I3TableRowDescriptionPtr CreateDescription(const I3Vector<T> &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");

    converter_type::AddFields(desc);

    return desc;
  }

  size_t FillRows(const I3Vector<T> &v, I3TableRowPtr rows)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    size_t index = 0;
    for(typename I3Vector<T>::const_iterator iter = v.begin();
	iter != v.end(); iter++)
      {
	rows->SetCurrentRow(index);
	rows->Set<tableio_size_t>("vector_index", index);
	
	converter_type::FillRow(*iter, rows);
	
	++index;
      } // loop over vector

    return index;
  }

};

#endif // TABLEIO_I3VECTORCONVERTER_H_INCLUDED
