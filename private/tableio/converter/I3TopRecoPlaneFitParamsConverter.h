/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TopRecoPlaneFitParamsConverter.h 87997 2012-05-07 00:07:52Z jvansanten $
 *
 * @version $Revision: 87997 $
 * @date $LastChangedDate: 2012-05-06 20:07:52 -0400 (Sun, 06 May 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de>, last changed by $LastChangedBy: jvansanten $
 */

#include <tableio/I3Converter.h>
#include <dataclasses/physics/I3TopRecoPlaneFitParams.h>

class I3TopRecoPlaneFitParamsConverter : public I3ConverterImplementation<I3TopRecoPlaneFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopRecoPlaneFitParams& params); 
    size_t FillRows(const I3TopRecoPlaneFitParams& params, I3TableRowPtr rows);
};
    
