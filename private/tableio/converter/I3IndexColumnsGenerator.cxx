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

//#include "tableio/I3Converter.h"
#include "tableio/I3ConverterFactory.h"
#include "dataclasses/physics/I3EventHeader.h"

class I3IndexColumnsGenerator : public I3ConverterImplementation<I3EventHeader> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3EventHeader& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc->AddField<uint32_t>("Run", "", "run number");
            desc->AddField<uint32_t>("Event", "", "event number");
            desc->AddField<bool>("exists", "", "object was found in the frame");
            return desc;
        }

        size_t FillRows(const I3EventHeader& header, I3TableRowPtr rows) {
            rows->Set<uint32_t>("Run", header.GetRunID());
            rows->Set<uint32_t>("Event", header.GetEventID());
            rows->Set<bool>("exists", false); // to be overwritten by someone else, e.g. I3TableWriter
            return 1;
        }
};

I3_CONVERTER(I3IndexColumnsGenerator, I3IndexColumnsGenerator);


