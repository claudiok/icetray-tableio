/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: dataclasses_map_converters.h 67236 2010-10-05 12:08:13Z kislat $
 *
 * @version $Revision: 67236 $
 * @date $LastChangedDate: 2010-10-05 14:08:13 +0200 (Tue, 05 Oct 2010) $
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy: kislat $
 */

#ifndef TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED
#define TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3FlasherInfo.h>
#include <tableio/I3TableRowDescription.h>
#include <tableio/I3TableRow.h>
#include <utility>

#include "pod_converter_type_mapping.h"


namespace convert {

  template <typename T>
  struct pod {
    typedef T booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type())
    {
      desc->AddField< typename detail::pod_converter_type_mapping<T>::type >("item", "", "Item in the booked vector");
    }

    void FillSingleRow(const booked_type &item, I3TableRowPtr row)
    {
      row->Set< typename detail::pod_converter_type_mapping<T>::type >("item", item);
    }
  };

  struct I3DOMLaunch
  { 
    typedef ::I3DOMLaunch booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type& dl, I3TableRowPtr row);
  };
  
  struct I3RecoHit
  {
    typedef ::I3RecoHit booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type& hit, I3TableRowPtr row);
  };
  
  struct I3MCHit
  {
    typedef ::I3MCHit booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type& hit, I3TableRowPtr row);
  };
  
  struct I3RecoPulse
  {
    typedef ::I3RecoPulse booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type& pulse, I3TableRowPtr row);
  };

  struct double_pair {
    typedef std::pair<double, double> booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type &item, I3TableRowPtr row);
  };

  struct I3FlasherInfo {
    typedef ::I3FlasherInfo booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type &flasherinfo, I3TableRowPtr row);
  };

  struct OMKey {
    typedef ::OMKey booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type &key, I3TableRowPtr row);
  };

  struct TankKey {
    typedef ::TankKey booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type());
    void FillSingleRow(const booked_type &key, I3TableRowPtr row);
  };

}

#endif // TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED
