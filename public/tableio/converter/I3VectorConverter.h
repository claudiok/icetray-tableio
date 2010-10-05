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


template <class converter_type,
	  typename value_type = typename converter_type::value_type >
class I3VectorConverter : public I3ConverterImplementation< I3Vector< value_type > > {
private:
  typedef I3Vector< value_type > vector_type;

  size_t GetNumberOfRows(const vector_type &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    return v.size();
  }

  I3TableRowDescriptionPtr CreateDescription(const vector_type &v)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");

    if (v.size()) {
      converter_type::AddFields(desc, v[0]);
    } else {
      converter_type::AddFields(desc);
    }

    return desc;
  }

  size_t FillRows(const vector_type &v, I3TableRowPtr rows)
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    size_t row = rows->GetCurrentRow();
    for(typename vector_type::const_iterator iter = v.begin();
	iter != v.end(); iter++)
      {
	rows->SetCurrentRow(row);
	rows->Set<tableio_size_t>("vector_index", row);
	
	converter_type::FillSingleRow(*iter, rows);
	
	++row;
      } // loop over vector

    return v.size();
  }

};

#endif // TABLEIO_I3VECTORCONVERTER_H_INCLUDED
