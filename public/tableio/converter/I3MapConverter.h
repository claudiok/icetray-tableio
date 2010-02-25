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
      desc->AddField<double>("StartTime", "ns/10", "start time");
    }

    static void FillRow(const I3DOMLaunch& launch, I3TableRowPtr row) 
    {
      row->Set<double>("StartTime", launch.GetStartTime());
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
    log_warn("%s", __PRETTY_FUNCTION__);
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
    log_warn("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<int>("String", "", "string number");
    desc->AddField<unsigned>("OM", "", "om number");
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
      
    converter_type::AddFields(desc);

    return desc;
  }

  size_t FillRows(const T& m, I3TableRowPtr rows) 
  {
    log_warn("%s", __PRETTY_FUNCTION__);
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
	    rows->Set<int>("String", mapiter->first.GetString());
	    rows->Set<unsigned>("OM", mapiter->first.GetOM());
	    rows->Set<tableio_size_t>("vector_index", vecindex);

	    converter_type::FillRow(*veciter, rows);
	    
	    log_warn("%u %d", mapiter->first.GetString(), mapiter->first.GetOM());


	    index++;
	    vecindex++;
	  }
      }
    // loop over vector
    return index;
  }

  std::string  converterName_;
};
