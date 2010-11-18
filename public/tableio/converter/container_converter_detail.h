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

#ifndef TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED
#define TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED

namespace detail {
  
  template <class Converter_T, class booked_type, class is_i3converter>
  struct add_fields_impl {
    static void do_add(Converter_T &converter, I3TableRowDescriptionPtr desc, const std::vector<booked_type> &v)
    {
      if (v.size()) {
	converter.AddFields(desc, v[0]);
      } else {
	converter.AddFields(desc);
      }
    }
  };

  template <class Converter_T, class booked_type>
  struct add_fields_impl<Converter_T, booked_type, boost::true_type> {
    static void do_add(Converter_T &converter, I3TableRowDescriptionPtr desc, const std::vector<booked_type> &v)
    {
      I3TableRowDescriptionConstPtr item_desc;
      if (v.size() > 0) {
	item_desc = converter.GetDescription(v[0]);
      } else {
	item_desc = converter.GetDescription(booked_type());
      }
      *desc << *item_desc;
    }
  };

  template <class Converter_T, class booked_type>
  void add_fields(Converter_T &converter, I3TableRowDescriptionPtr desc, const std::vector<booked_type> &v)
  {
    add_fields_impl< Converter_T, booked_type, typename boost::is_base_of<I3Converter, Converter_T>::type >::do_add(converter, desc, v);
  }


  template <class Converter_T, class booked_type, class is_i3converter>
  struct fill_single_row_impl {
    static void do_fill(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr)
    {
      converter.FillSingleRow(value, rows);
    }
  };

  template <class Converter_T, class booked_type>
  struct fill_single_row_impl<Converter_T, booked_type, boost::true_type> {
    static void do_fill(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr frame)
    {
      converter.Convert(value, rows, frame);
    }
  };

  template <class Converter_T, class booked_type>
  void fill_single_row(Converter_T &converter, const booked_type &value, I3TableRowPtr rows, I3FramePtr frame)
  {
    fill_single_row_impl< Converter_T, booked_type, typename boost::is_base_of<I3Converter, Converter_T>::type >::do_fill(converter, value, rows, frame);
  }

}

#endif // TABLEIO_CONTAINER_CONVERTER_DETAIL_H_INCLUDED
