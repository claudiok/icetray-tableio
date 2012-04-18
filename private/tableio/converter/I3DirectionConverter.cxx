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
    
    if(book_ref_frame_ == I3DirectionConverter::car)
    {
        desc->AddField<double>("x", "", "x-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>("y", "", "y-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>("z", "", "z-component of the direction as an unit vector in the cartesian reference frame");
    }
    else if(book_ref_frame_ == I3DirectionConverter::sph)
    {
        desc->AddField<double>("azimuth", "radian", "azimuth angle of the direction in the spherical reference frame");
        desc->AddField<double>("zenith",  "radian", "zenith angle of the direction in the spherical reference frame");
    }
    else if(book_ref_frame_ == I3DirectionConverter::all)
    {
        desc->AddField<double>("x",       "",       "x-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>("y",       "",       "y-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>("z",       "",       "z-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>("azimuth", "radian", "azimuth angle of the direction in the spherical reference frame");
        desc->AddField<double>("zenith",  "radian", "zenith angle of the direction in the spherical reference frame");
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", book_ref_frame_);
    }
    
    return desc;
};

//______________________________________________________________________________
size_t
I3DirectionConverter::
FillRows(const I3Direction& direction, I3TableRowPtr row)
{
    if(book_ref_frame_ == I3DirectionConverter::car)
    {
        row->Set<double>("x", direction.GetX());
        row->Set<double>("y", direction.GetY());
        row->Set<double>("z", direction.GetZ());
    }
    else if(book_ref_frame_ == I3DirectionConverter::sph)
    {
        row->Set<double>("azimuth", direction.GetAzimuth());
        row->Set<double>("zenith",  direction.GetZenith());
    }
    else if(book_ref_frame_ == I3DirectionConverter::all)
    {
        row->Set<double>("x",       direction.GetX());
        row->Set<double>("y",       direction.GetY());
        row->Set<double>("z",       direction.GetZ());
        row->Set<double>("azimuth", direction.GetAzimuth());
        row->Set<double>("zenith",  direction.GetZenith());
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", book_ref_frame_);
    }
    
    return 1;
};

//______________________________________________________________________________
I3_CONVERTER(I3DirectionConverter, I3Direction);

