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

#include <tableio/I3ConverterFactory.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3FlasherInfo.h>


namespace vector_converter {

  template <typename T> struct convert;

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
    size_t row = rows->GetCurrentRow();
    for(typename I3Vector<T>::const_iterator iter = v.begin();
	iter != v.end(); iter++)
      {
	rows->SetCurrentRow(row);
	rows->Set<tableio_size_t>("vector_index", row);
	
	converter_type::FillRow(*iter, rows);
	
	++row;
      } // loop over vector

    return v.size();
  }

};

#endif // TABLEIO_I3VECTORCONVERTER_H_INCLUDED
