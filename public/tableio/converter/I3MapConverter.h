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

#include <tableio/I3Converter.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/OMKey.h>
#include <tableio/converter/container_converter_detail.h>


template <class converter_type,
	  typename map_type = I3Map<OMKey, std::vector<typename converter_type::booked_type> >,
          typename frameobject_type = map_type >
class I3MapOMKeyVectorConverter 
  : public I3ConverterImplementation< frameobject_type > 
{
public:
  I3MapOMKeyVectorConverter(bool bookGeometry = false)
    : bookGeometry_(bookGeometry)
  {}

protected:
  size_t GetNumberOfRows(const map_type& m) {
    log_trace("%s", __PRETTY_FUNCTION__);
    typename map_type::const_iterator iter = m.begin();
    size_t nrows = 0;
    while (iter != m.end())
      {
	nrows += iter->second.size();
	++iter;
      }
    return nrows;
  }

  I3TableRowDescriptionPtr CreateDescription(const map_type& m) 
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<int32_t>("string", "", "String number");
    desc->AddField<uint32_t>("om", "", "OM number");
    if (bookGeometry_) {
      desc->AddField<double>("x", "m", "X coordinate of the DOM");
      desc->AddField<double>("y", "m", "Y coordinate of the DOM");
      desc->AddField<double>("z", "m", "Z coordinate of the DOM");
    }
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
      
    if (m.size() && m.begin()->second.size()) {
      detail::add_fields(converter_, desc, *(m.begin()->second.begin()));
    } else {
      typedef typename map_type::mapped_type::value_type value_type;
      detail::add_fields(converter_, desc, value_type());
    }

    return desc;
  }

  size_t FillRows(const map_type& m, I3TableRowPtr rows) 
  {
    static int nGeometryWarnings = 0;

    log_trace("%s", __PRETTY_FUNCTION__);
    size_t index = 0;

    I3GeometryConstPtr geometry;
    if (bookGeometry_) {
      if (!this->currentFrame_)  // obsolete check?
	log_fatal("Trying to book geometry, but the current frame is not set.");
      geometry = this->currentFrame_->template Get<I3GeometryConstPtr>();
      if (!geometry) {
	log_error("%s: No geometry in frame", __PRETTY_FUNCTION__);
	return 0;
      }
    }

    for(typename map_type::const_iterator mapiter = m.begin();
	mapiter != m.end();
	mapiter++)
      {

	OMKey key = mapiter->first;
	I3OMGeo omgeo;

	if (bookGeometry_) {
	  I3OMGeoMap::const_iterator geoiter = geometry->omgeo.find(key);
	  if (geoiter == geometry->omgeo.end()) {
	    log_warn("%s: OMKey (%d,%d) not in geometry!", __PRETTY_FUNCTION__,
		     key.GetString(), key.GetOM());
	    ++nGeometryWarnings;
	    if (nGeometryWarnings >= 100)
	      log_info("Warned 100 times. Will suppress any further warnings.");
	  } else {
	    omgeo = geoiter->second;
	  }
	}

	int vecindex = 0;
	for (typename map_type::mapped_type::const_iterator veciter = mapiter->second.begin();
	     veciter != mapiter->second.end();
	     veciter++)
	  {
	    rows->SetCurrentRow(index);
	    rows->Set<int32_t>("string", mapiter->first.GetString());
	    rows->Set<uint32_t>("om", mapiter->first.GetOM());
	    if (bookGeometry_) {
	      rows->Set<double>("x", omgeo.position.GetX());
	      rows->Set<double>("y", omgeo.position.GetY());
	      rows->Set<double>("z", omgeo.position.GetZ());
	    }
	    rows->Set<tableio_size_t>("vector_index", vecindex);

	    detail::fill_single_row(converter_, *veciter, rows, this->currentFrame_);
	    
	    log_trace("String: %d OM: %d", mapiter->first.GetString(), mapiter->first.GetOM());

	    index++;
	    vecindex++;
	  }
      }
    // loop over vector
    return index;
  }

private:
  bool bookGeometry_;
  converter_type converter_;
};


#define I3_MAP_CONVERTER_EXPORT(converter, docstring)			\
  I3CONVERTER_EXPORT(converter, docstring)				\
  .def(boost::python::init<bool>(boost::python::args("bookGeometry")))

#define I3_MAP_CONVERTER_EXPORT_DEFAULT(converter, docstring)		\
  I3CONVERTER_EXPORT_DEFAULT(converter, docstring)			\
  .def(boost::python::init<bool>(boost::python::args("bookGeometry")))

#endif // TABLEIO_I3MAPCONVERTER_H_INCLUDED
