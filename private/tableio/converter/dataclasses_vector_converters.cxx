/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/I3ConverterFactory.h"
#include "tableio/converter/dataclasses_vector_converters.h"

#include <boost/preprocessor.hpp>


namespace convert_vector {

  void double_pair::AddFields(I3TableRowDescriptionPtr desc)
  {
    desc->AddField< double >("first", "", "Vector of double-double pair: first item");
    desc->AddField< double >("second", "", "Vector of double-double pair: second item");
  }

  void double_pair::FillSingleRow(const value_type &item, I3TableRowPtr row)
  {
    row->Set< double >("first", item.first);
    row->Set< double >("second", item.second);
  }


  void I3FlasherInfo::AddFields(I3TableRowDescriptionPtr desc)
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

  void I3FlasherInfo::FillSingleRow(const value_type &flasherinfo, I3TableRowPtr row)
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

}


I3_CONVERTER(I3VectorConverter< convert_vector::pod<bool> >, I3Vector<bool>);

I3_CONVERTER(I3VectorConverter< convert_vector::pod<int16_t> >, I3Vector<int16_t>);
I3_CONVERTER(I3VectorConverter< convert_vector::pod<uint16_t> >, I3Vector<uint16_t>);

I3_CONVERTER(I3VectorConverter< convert_vector::pod<int32_t> >, I3Vector<int32_t>);
I3_CONVERTER(I3VectorConverter< convert_vector::pod<uint32_t> >, I3Vector<uint32_t>);

I3_CONVERTER(I3VectorConverter< convert_vector::pod<I3VectorInt64::value_type> >, I3Vector<I3VectorInt64::value_type>);
I3_CONVERTER(I3VectorConverter< convert_vector::pod<I3VectorUInt64::value_type> >, I3Vector<I3VectorUInt64::value_type>);

I3_CONVERTER(I3VectorConverter< convert_vector::pod<float> >, I3Vector<float>);
I3_CONVERTER(I3VectorConverter< convert_vector::pod<double> >, I3Vector<double>);

typedef I3VectorConverter< convert_vector::double_pair > I3VectorDoubleDoubleConverter;
I3_CONVERTER(I3VectorDoubleDoubleConverter, I3VectorDoubleDouble);

I3_CONVERTER(I3VectorConverter< convert_vector::I3FlasherInfo >, I3FlasherInfoVect);
