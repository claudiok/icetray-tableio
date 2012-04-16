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

#include "tableio/converter/I3PositionConverter.h"

//______________________________________________________________________________
I3TableRowDescriptionPtr
I3PositionConverter::
CreateDescription(const I3Position& position)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<double>("x",     "m",      "x-component of position in the cartesian reference frame");
    desc->AddField<double>("y",     "m",      "y-component of position in the cartesian reference frame");
    desc->AddField<double>("z",     "m",      "z-component of position in the cartesian and cylindrical reference frame");
    desc->AddField<double>("r",     "m",      "radius of position in the spherical reference frame");
    desc->AddField<double>("theta", "radian", "theta angle of position in the spherical reference frame");
    desc->AddField<double>("phi",   "radian", "phi angle of position in the spherical and cylindrical reference frame");
    desc->AddField<double>("rho",   "m",      "radius of position in the cylindrical reference frame");
    
    return desc;
};

//______________________________________________________________________________
size_t
I3PositionConverter::
FillRows(const I3Position& position, I3TableRowPtr row)
{
    row->Set<double>("x",     position.GetX());
    row->Set<double>("y",     position.GetY());
    row->Set<double>("z",     position.GetZ());
    row->Set<double>("r",     position.GetR());
    row->Set<double>("theta", position.GetTheta());
    row->Set<double>("phi",   position.GetPhi());
    row->Set<double>("rho",   position.GetRho());
    
    return 1;
};

//______________________________________________________________________________
I3_CONVERTER(I3PositionConverter, I3Position);

