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

#ifndef TABLEIO_DATACLASSES_MAP_CONVERTERS_H_INCLUDED
#define TABLEIO_DATACLASSES_MAP_CONVERTERS_H_INCLUDED

#include "tableio/converter/I3MapConverter.h"

#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3MCHit.h>


namespace convert {

  struct I3DOMLaunch
  { 
    typedef ::I3DOMLaunch value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type& dl, I3TableRowPtr row);
  };
  
  struct I3RecoHit
  {
    typedef ::I3RecoHit value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type& hit, I3TableRowPtr row);
  };
  
  struct I3MCHit
  {
    typedef ::I3MCHit value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type& hit, I3TableRowPtr row);
  };
  
  struct I3RecoPulse
  {
    typedef ::I3RecoPulse value_type;

    void AddFields(I3TableRowDescriptionPtr desc, const value_type& = value_type());
    void FillSingleRow(const value_type& pulse, I3TableRowPtr row);
  };

}

#endif // TABLEIO_DATACLASSES_MAP_CONVERTERS_H_INCLUDED
