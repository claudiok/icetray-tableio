/**
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Martin Wolf <martin.wolf@fysik.su.se> $LastChangedBy$
 */

#include "tableio/converter/I3DirectionConverter.h"

//______________________________________________________________________________
I3TableRowDescriptionPtr
I3DirectionConverter::
CreateDescription(const I3Direction& direction)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<double>("azimuth", "radian", "azimuth angle of the direction in a spherical reference frame");
    desc->AddField<double>("zenith",  "radian", "zenith angle of the direction in a spherical reference frame");
    desc->AddField<double>("x",       "m",      "x-component of the direction as an unit vector in a cartesian reference frame");
    desc->AddField<double>("y",       "m",      "y-component of the direction as an unit vector in a cartesian reference frame");
    desc->AddField<double>("z",       "m",      "z-component of the direction as an unit vector in a cartesian reference frame");
    
    return desc;
};

//______________________________________________________________________________
size_t
I3DirectionConverter::
FillRows(const I3Direction& direction, I3TableRowPtr row)
{
    row->Set<double>("azimuth", direction.GetAzimuth());
    row->Set<double>("zenith",  direction.GetZenith());
    row->Set<double>("x",       direction.GetX());
    row->Set<double>("y",       direction.GetY());
    row->Set<double>("z",       direction.GetZ());
    
    return 1;
};

//______________________________________________________________________________
I3_CONVERTER(I3DirectionConverter, I3Direction);

