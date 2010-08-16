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

// A converter can be made for frame objects of the form map<OMKey, vector<T>> by
// specializing the following templated structs for T. 

// the converters should be written to be indifferent on the state of the I3TableRow.
// So if an I3DOMLaunch converter exists, converters for vector<I3DOMLaunch> and
// map<omkey, vector<I3DOMLaunch>> could be obtained for free.
//
// FIXME: vector<I3DOMLaunch> is no I3FrameObject. how to get this through the I3Converter
//        interface?

#ifndef TABLEIO_I3MAPCONVERTER_H_INCLUDED
#define TABLEIO_I3MAPCONVERTER_H_INCLUDED

#include <tableio/I3ConverterFactory.h>
#include <dataclasses/I3Map.h>
#include <icetray/OMKey.h>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include "dataclasses/physics/I3MCHit.h"


namespace converter
{

  template <typename T>
  struct convert { };

}

template <class T, 
	  typename mapped_type = typename T::mapped_type::value_type,
	  typename converter_type = typename converter::convert<mapped_type> >
class I3MapOMKeyVectorConverter 
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
    desc->AddField<int8_t>("string", "", "String number");
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
	    rows->Set<int8_t>("string", mapiter->first.GetString());
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

#endif // TABLEIO_I3MAPCONVERTER_H_INCLUDED
