/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: dataclasses_map_converters.cxx 67231 2010-10-05 08:36:34Z kislat $
 *
 * @version $Revision: 67231 $
 * @date $LastChangedDate: 2010-10-05 10:36:34 +0200 (Tue, 05 Oct 2010) $
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy: kislat $
 */

#include "dataclasses_container_convert.h"

namespace convert {

  void I3DOMLaunch::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("start_time","ns","Time at which the DOM went over threshold");
    desc->AddField<bool>("pedestal_sub","bool","Has the pedestal been subtracted from the waveform?");
    desc->AddField<bool>("lc_bit","bool","Was the local-coincidence condition satistfied?");
    
    MAKE_ENUM_VECTOR(atwdselect, ::I3DOMLaunch, ATWDselect, I3DOMLAUNCH_H_I3DOMLaunch_ATWDselect);
    desc->AddEnumField< ::I3DOMLaunch::ATWDselect >("which_atwd", atwdselect, "", "Which ATWD chip recorded this launch?");
    MAKE_ENUM_VECTOR(trigger_type, ::I3DOMLaunch, TriggerType, I3DOMLAUNCH_H_I3DOMLaunch_TriggerType);
    desc->AddEnumField< ::I3DOMLaunch::TriggerType>("trigger_type", trigger_type, "", "");     
    MAKE_ENUM_VECTOR(trigger_mode, ::I3DOMLaunch, TriggerMode, I3DOMLAUNCH_H_I3DOMLaunch_TriggerMode);
    desc->AddEnumField< ::I3DOMLaunch::TriggerMode>("trigger_mode", trigger_mode, "", "");
    
    desc->AddField<uint16_t>("raw_atwd_0", "counts", "Raw digitizer counts from ATWD channel 0", 128);
    desc->AddField<uint16_t>("raw_atwd_1", "counts", "Raw digitizer counts from ATWD channel 1", 128);
    desc->AddField<uint16_t>("raw_atwd_2", "counts", "Raw digitizer counts from ATWD channel 2", 128);
    desc->AddField<uint16_t>("raw_charge_stamp", "counts",
			     "The number of counts in highest-charge bin of the first 16 fADC bins (400 ns), plus the bins immediately before and after.",
			     3);
    desc->AddField<uint32_t>("raw_atwd_charge_stamp", "counts", "ATWD charge stamp used for IceTop style SLC readout");
    desc->AddField<uint8_t>("raw_atwd_charge_stamp_channel", "atwd_channel", "ATWD channel used for the ATWD charge stamp");
    desc->AddField<uint16_t>("raw_fadc", "counts", "Raw digitizer counts from fADC", 256);
  }

  void I3DOMLaunch::FillSingleRow(const booked_type& dl, I3TableRowPtr row) 
  {
    row->Set<double>("start_time", dl.GetStartTime());
    row->Set< ::I3DOMLaunch::ATWDselect>("which_atwd",dl.GetWhichATWD());
    row->Set< ::I3DOMLaunch::TriggerMode>("trigger_mode",dl.GetTriggerMode());
    row->Set< ::I3DOMLaunch::TriggerType>("trigger_type",dl.GetTriggerType());
    row->Set<bool>("pedestal_sub", dl.GetIsPedestalSub());
    row->Set<bool>("lc_bit", dl.GetLCBit());
    row->Set<uint32_t>("raw_atwd_charge_stamp", dl.GetRawATWDChargeStamp());
    row->Set<uint8_t>("raw_atwd_charge_stamp_channel", dl.GetWhichATWDChargeStamp());
    std::vector<int32_t>::const_iterator it;
    size_t i; uint16_t* pointy;
      
    pointy = row->GetPointer<uint16_t>("raw_atwd_0");
    for (it = dl.GetRawATWD(0).begin(), i = 0; it != dl.GetRawATWD(0).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_atwd_1");
    for (it = dl.GetRawATWD(1).begin(), i = 0; it != dl.GetRawATWD(1).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_atwd_2");
    for (it = dl.GetRawATWD(2).begin(), i = 0; it != dl.GetRawATWD(2).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_charge_stamp");
    for (it = dl.GetRawChargeStamp().begin(), i = 0; it != dl.GetRawChargeStamp().end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_fadc");
    for (it = dl.GetRawFADC().begin(), i = 0; it != dl.GetRawFADC().end(); it++, i++) {
      pointy[i] = *it;
    } 
  }


  void I3RecoHit::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<int32_t>("id", "generic", "hit id");
  }

  void I3RecoHit::FillSingleRow(const booked_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<int32_t>("id", hit.GetID());
  }


  void I3MCHit::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<double>("weight", "PE", "The number of photoelectrons the hit represents.");
    desc->AddField<double>("cherenkov_distance", "m", "FIXME: document");
    MAKE_ENUM_VECTOR(hit_source, ::I3MCHit, HitSource, I3MCHIT_H_I3MCHit_HitSource);
    desc->AddEnumField< ::I3MCHit::HitSource>("source",hit_source,"","");
    
    // skipped: hitID, particleMajorID, particleMinorID
    
  }

  void I3MCHit::FillSingleRow(const booked_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<double>("weight", hit.GetWeight());
    row->Set<double>("cherenkov_distance", hit.GetCherenkovDistance());
    row->Set< ::I3MCHit::HitSource>("source", hit.GetHitSource());
  }


  void I3RecoPulse::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "Leading-edge time of the pulse");
    desc->AddField<double>("width", "ns", "Duration of the pulse");
    desc->AddField<double>("charge", "PE", "Integrated pulse charge");
    desc->AddField<int32_t>("id", "generic", "hit id");
  }

  void I3RecoPulse::FillSingleRow(const booked_type& pulse, I3TableRowPtr row)
  {
    row->Set<double>("time", pulse.GetTime());
    row->Set<double>("width", pulse.GetWidth());
    row->Set<double>("charge", pulse.GetCharge());
    row->Set<int32_t>("id", pulse.GetID());
  }


  void double_pair::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField< double >("first", "", "Vector of double-double pair: first item");
    desc->AddField< double >("second", "", "Vector of double-double pair: second item");
  }

  void double_pair::FillSingleRow(const booked_type &item, I3TableRowPtr row)
  {
    row->Set< double >("first", item.first);
    row->Set< double >("second", item.second);
  }


  void I3FlasherInfo::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<int8_t>("string", "", "String number of the flashing DOM");
    desc->AddField<uint8_t>("om", "", "OM number of the flashing DOM");
    desc->AddField<double>("flash_time", "ns", "Time (in nsec) in 25 nsec units, of the LED flash time.");
    desc->AddField<uint32_t>("mask", "bitmask", "Indicates which LEDs were flashing");
    // #warning TODO: improve doc strings
    desc->AddField<int32_t>("width", "", "Flasher width");
    desc->AddField<int32_t>("rate", "", "Flasher rate");
    desc->AddField<int32_t>("brightness", "", "Brightness level (1..127)");
    
    // Maybe move those to another converter:
    desc->AddField<double>("atwd_bin_size", "ns", "Bin width of the ATWD3");
    desc->AddField<int32_t>("raw_atwd3", "counts", "ATWD waveform of the LED current pulse", 128);  // <-- a vector!
  }

  void I3FlasherInfo::FillSingleRow(const booked_type &flasherinfo, I3TableRowPtr row)
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


  void OMKey::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<int8_t>("string", "", "String number");
    desc->AddField<uint8_t>("om", "", "OM number");
  }

  void OMKey::FillSingleRow(const booked_type &key, I3TableRowPtr row)
  {
    row->Set<int8_t>("string", key.GetString());
    row->Set<uint8_t>("om", key.GetOM());
  }

}
