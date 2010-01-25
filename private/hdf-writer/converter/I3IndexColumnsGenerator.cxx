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

//#include "hdf-writer/I3Converter.h"
#include "hdf-writer/internals/I3ConverterFactory.h"
#include "dataclasses/physics/I3EventHeader.h"

class I3IndexColumnsGenerator : public I3ConverterImplementation<I3EventHeader> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3EventHeader& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc->AddField<unsigned int>("Run", "", "run number");
            desc->AddField<unsigned int>("Event", "", "event number");
            desc->AddField<bool>("exists", "", "object was found in the frame");
            return desc;
        }

        unsigned int FillRows(const I3EventHeader& header, I3TableRowPtr rows) {
            rows->Set<unsigned int>("Run", header.GetRunID());
            rows->Set<unsigned int>("Event", header.GetEventID());
            rows->Set<bool>("exists", false); // to be overwritten by someone else, e.g. I3TableWriter
            return 1;
        }
};

I3_CONVERTER(I3IndexColumnsGenerator, I3IndexColumnsGenerator);


