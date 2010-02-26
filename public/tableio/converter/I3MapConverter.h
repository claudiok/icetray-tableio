/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3STLContainerConverter.h 61800 2010-02-23 02:22:49Z jvansanten $
 *
 * @version $Revision: 61800 $
 * @date $LastChangedDate: 2010-02-22 21:22:49 -0500 (Mon, 22 Feb 2010) $
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy: jvansanten $
 */

// the converters should be written to be indifferent on the state of the I3TableRow.
// So if an I3DOMLaunch converter exists, converters for vector<I3DOMLaunch> and
// map<omkey, vector<I3DOMLaunch>> could be obtained for free.
//
// FIXME: vector<I3DOMLaunch> is no I3FrameObject. how to get this through the I3Converter
//        interface?

#include <tableio/internals/I3ConverterFactory.h>
#include <dataclasses/I3Map.h>
#include <icetray/OMKey.h>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>

namespace converter
{
  template <typename T>
  struct convert { };


  template <>
  struct convert<I3DOMLaunch> 
  { 
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField<double>("start_time","ns/10","Time at which the DOM was triggered");
      desc->AddField<bool>("pedestal_sub","bool","Has the pedestal been subtracted from the waveform?");
      desc->AddField<bool>("lc_bit","bool","Was the local-cooincidence condition satistfied?");
      
      MAKE_ENUM_VECTOR(atwdselect,I3DOMLaunch,I3DOMLaunch::ATWDselect,I3DOMLAUNCH_H_I3DOMLaunch_ATWDselect);
      desc->AddEnumField<I3DOMLaunch::ATWDselect>("which_atwd",atwdselect,"","Which ATWD chip recorded this launch?");     
      MAKE_ENUM_VECTOR(trigger_type,I3DOMLaunch,I3DOMLaunch::TriggerType,I3DOMLAUNCH_H_I3DOMLaunch_TriggerType);
      desc->AddEnumField<I3DOMLaunch::TriggerType>("trigger_type",trigger_type,"","");     
      MAKE_ENUM_VECTOR(trigger_mode,I3DOMLaunch,I3DOMLaunch::TriggerMode,I3DOMLAUNCH_H_I3DOMLaunch_TriggerMode);
      desc->AddEnumField<I3DOMLaunch::TriggerMode>("trigger_mode",trigger_mode,"","");
      
      desc->AddField<uint16_t>("raw_atwd_0","counts","Raw digitizer counts from ATWD channel 0",128);
      desc->AddField<uint16_t>("raw_atwd_1","counts","Raw digitizer counts from ATWD channel 1",128);
      desc->AddField<uint16_t>("raw_atwd_2","counts","Raw digitizer counts from ATWD channel 2",128);
      desc->AddField<uint16_t>("raw_charge_stamp","counts",
            "The counts of highest-charge bin of the first 16 fADC bins (400 ns), plus the bins immediately before and after.",3);
      desc->AddField<uint16_t>("raw_fadc","counts","Raw digitizer counts from fADC",256);
    }

    static void FillRow(const I3DOMLaunch& dl, I3TableRowPtr row) 
    {
      row->Set<double>("start_time", dl.GetStartTime());
      row->Set<I3DOMLaunch::ATWDselect>("which_atwd",dl.GetWhichATWD());
      row->Set<I3DOMLaunch::TriggerMode>("trigger_mode",dl.GetTriggerMode());
      row->Set<I3DOMLaunch::TriggerType>("trigger_type",dl.GetTriggerType());
      row->Set<bool>("pedestal_sub", dl.GetIsPedestalSub());
      row->Set<bool>("lc_bit", dl.GetLCBit());
      std::vector<int>::const_iterator it;
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
  };
  
  template <>
  struct convert<I3RecoHit> 
  { 
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField<double>("Time", "ns/10", "time");
      desc->AddField<int>("ID", "generic", "hit id");
    }

    static void FillRow(const I3RecoHit& hit, I3TableRowPtr row) 
    {
      row->Set<double>("Time", hit.GetTime());
      row->Set<int>("ID", hit.GetID());
    }
  };

  template <>
  struct convert<I3RecoPulse> 
  { 
    static void AddFields(I3TableRowDescriptionPtr desc)
    {
      desc->AddField<double>("Time", "ns/10", "time");
      desc->AddField<int>("ID", "generic", "hit id");
      desc->AddField<double>("Charge", "pes", "charge in PEs");
    }

    static void FillRow(const I3RecoPulse& pulse, I3TableRowPtr row) 
    {
      row->Set<double>("Time", pulse.GetTime());
      row->Set<int>("ID", pulse.GetID());
      row->Set<double>("Charge", pulse.GetCharge());
    }
  };
}

template <class T, 
	  typename mapped_type = typename T::mapped_type::value_type,
	  typename converter_type = typename converter::convert<mapped_type> >
class I3MapOMKeyConverter 
  : public I3ConverterImplementation<I3Map<OMKey, std::vector<mapped_type> > > 
{

private:
  size_t GetNumberOfRows(const T& m) {
    log_trace("%s", __PRETTY_FUNCTION__);
    typename T::const_iterator iter = m.begin();
    size_t nrows = 0;
    while (iter != m.end())
      {
	nrows += iter->second.size();
	++iter;
      }
    return nrows;
  }

  I3TableRowDescriptionPtr CreateDescription(const T& m) 
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<uint8_t>("string", "", "String number");
    desc->AddField<uint8_t>("om", "", "OM number");
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
      
    converter_type::AddFields(desc);

    return desc;
  }

  size_t FillRows(const T& m, I3TableRowPtr rows) 
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    size_t index = 0;
    for(typename T::const_iterator mapiter = m.begin();
	mapiter != m.end();
	mapiter++)
      {
	int vecindex = 0;
	for (typename T::mapped_type::const_iterator veciter = mapiter->second.begin();
	     veciter != mapiter->second.end();
	     veciter++)
	  {
	    rows->SetCurrentRow(index);
	    rows->Set<uint8_t>("string", mapiter->first.GetString());
	    rows->Set<uint8_t>("om", mapiter->first.GetOM());
	    rows->Set<tableio_size_t>("vector_index", vecindex);

	    converter_type::FillRow(*veciter, rows);
	    
	    log_trace("String: %d OM: %d", mapiter->first.GetString(), mapiter->first.GetOM());


	    index++;
	    vecindex++;
	  }
      }
    // loop over vector
    return index;
  }

  std::string  converterName_;
};
