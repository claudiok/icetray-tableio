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
#include "tableio/converter/dataclasses_map_converters.h"

namespace convert {

  void I3DOMLaunch::AddFields(I3TableRowDescriptionPtr desc)
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

  void I3DOMLaunch::FillSingleRow(const value_type& dl, I3TableRowPtr row) 
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


  void I3RecoHit::AddFields(I3TableRowDescriptionPtr desc)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<int32_t>("id", "generic", "hit id");
  }

  void I3RecoHit::FillSingleRow(const value_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<int32_t>("id", hit.GetID());
  }


  void I3MCHit::AddFields(I3TableRowDescriptionPtr desc)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<double>("weight", "PE", "The number of photoelectrons the hit represents.");
    desc->AddField<double>("cherenkov_distance", "m", "FIXME: document");
    MAKE_ENUM_VECTOR(hit_source, ::I3MCHit, HitSource, I3MCHIT_H_I3MCHit_HitSource);
    desc->AddEnumField< ::I3MCHit::HitSource>("source",hit_source,"","");
    
    // skipped: hitID, particleMajorID, particleMinorID
    
  }

  void I3MCHit::FillSingleRow(const value_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<double>("weight", hit.GetWeight());
    row->Set<double>("cherenkov_distance", hit.GetCherenkovDistance());
    row->Set< ::I3MCHit::HitSource>("source", hit.GetHitSource());
  }


  void I3RecoPulse::AddFields(I3TableRowDescriptionPtr desc)
  {
    desc->AddField<double>("time", "ns", "Leading-edge time of the pulse");
    desc->AddField<double>("width", "ns", "Duration of the pulse");
    desc->AddField<double>("charge", "PE", "Integrated pulse charge");
    desc->AddField<int32_t>("id", "generic", "hit id");
  }

  void I3RecoPulse::FillSingleRow(const value_type& pulse, I3TableRowPtr row)
  {
    row->Set<double>("time", pulse.GetTime());
    row->Set<double>("width", pulse.GetWidth());
    row->Set<double>("charge", pulse.GetCharge());
    row->Set<int32_t>("id", pulse.GetID());
  }
}

I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3DOMLaunch >, I3DOMLaunchSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3RecoPulse >, I3RecoPulseSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3RecoHit >, I3RecoHitSeriesMap);
I3_CONVERTER(I3MapOMKeyVectorConverter< convert::I3MCHit >, I3MCHitSeriesMap);

