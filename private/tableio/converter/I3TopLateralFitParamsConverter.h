/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TopLateralFitParamsConverter.h 87997 2012-05-07 00:07:52Z jvansanten $
 *
 * @version $Revision: 87997 $
 * @date $LastChangedDate: 2012-05-06 20:07:52 -0400 (Sun, 06 May 2012) $
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy: jvansanten $
 */

#include <tableio/I3Converter.h>
#include <dataclasses/physics/I3TopLateralFitParams.h>

class I3TopLateralFitParamsConverter : public I3ConverterImplementation<I3TopLateralFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopLateralFitParams& params); 
    size_t FillRows(const I3TopLateralFitParams& params, I3TableRowPtr rows);
};
    
