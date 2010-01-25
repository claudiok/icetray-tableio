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

#include "hdf-writer/internals/I3Converter.h"

/******************************************************************************/

void I3Converter::Configure( const StringPairVector& params) { 
    if (params.size() > 0)
        log_fatal("This converter doesn't take any parameters!");
}

/******************************************************************************/

I3TableRowDescriptionConstPtr I3Converter::GetDescription() {
    if (description_)
        return description_;
    else
        log_fatal("description has not been created yet!");
}

/******************************************************************************/
