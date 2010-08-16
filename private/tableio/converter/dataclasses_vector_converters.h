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


  template <>
  struct convert< I3FlasherInfo > {
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField<int8_t>("string", "", "String number of the flashing DOM");
      desc->AddField<uint8_t>("om", "", "OM number of the flashing DOM");
      desc->AddField<double>("flash_time", "ns", "Time (in nsec) in 25 nsec units, of the LED flash time.");
      desc->AddField<uint32_t>("mask", "bitmask", "Indicates which LEDs were flashing");
#warning TODO: improve doc strings
      desc->AddField<int32_t>("width", "", "Flasher width");
      desc->AddField<int32_t>("rate", "", "Flasher rate");
      desc->AddField<int32_t>("brightness", "", "Brightness level (1..127)");
      
      // Maybe move those to another converter:
      desc->AddField<double>("atwd_bin_size", "ns", "Bin width of the ATWD3");
      desc->AddField<int32_t>("raw_atwd3", "counts", "ATWD waveform of the LED current pulse", 128);  // <-- a vector!
    }

    static void FillRow(const I3FlasherInfo &flasherinfo, I3TableRowPtr row)
    {
      row->Set<int8_t>("string", flasherinfo.GetFlashingOM().GetString());
      row->Set<uint8_t>("om", flasherinfo.GetFlashingOM().GetOM());
      row->Set<double>("flash_time", flasherinfo.GetFlashTime());
      row->Set<uint32_t>("mask", flasherinfo.GetMask());
      row->Set<int32_t>("width", flasherinfo.GetWidth());
      row->Set<int32_t>("rate", flasherinfo.GetRate());
      row->Set<int32_t>("brightness", flasherinfo.GetLEDBrightness());
      row->Set<double>("atwd_bin_size", flasherinfo.GetATWDBinSize());
      
      const vector<int> &waveform(flasherinfo.GetRawATWD3());
      size_t bins = std::max(waveform.size(), size_t(128));   // maybe paranoid, but safer
      int32_t *buffer = row->GetPointer<int32_t>("raw_atwd3");
      for (size_t i = 0; i < bins; ++i) {
	buffer[i] = waveform[i];
      }
    }
  };

}

#endif // TABLEIO_DATACLASSES_VECTOR_CONVERTERS_H_INCLUDED
