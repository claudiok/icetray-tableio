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

#include "tableio/converter/I3IndexColumnsGenerator.h"
#include <boost/foreach.hpp>

I3IndexColumnsGenerator::I3IndexColumnsGenerator(const std::vector<std::string> &streams)
{
	int num = 0;
	BOOST_FOREACH(const std::string &stream, streams)
		streams_[stream] = num++;
}

I3TableRowDescriptionPtr
I3IndexColumnsGenerator::CreateDescription(const I3EventHeader& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );

	/* You want something done right, you got to do it yourself. */
	I3Datatype enum_type;
	enum_type.size = sizeof(int32_t);
	enum_type.kind = I3Datatype::Enum;
	enum_type.is_signed = true;
	enum_type.description = "";
	
	/* XXX HACK: Replace "" with "NULL" in the enum description */ 
	BOOST_FOREACH(const stream_map_t::value_type &pair, streams_)
		if (pair.first.size() == 0)
			enum_type.enum_members.push_back(
			    std::make_pair(std::string("NULL"), pair.second));
		else
			enum_type.enum_members.push_back(pair);

            desc->AddField<uint32_t>("Run", "", "run number");
            desc->AddField<uint32_t>("Event", "", "event number");
            desc->AddField<uint32_t>("SubEvent", "", "sub-event number");
            desc->AddField("SubEventStream", enum_type, "",
                "ID of the splitter module that made this stream", 1);
            desc->AddField<bool>("exists", "", "object was found in the frame");
            return desc;
        }

        size_t
I3IndexColumnsGenerator::FillRows(const I3EventHeader& header, I3TableRowPtr rows) {
            rows->Set<uint32_t>("Run", header.GetRunID());
            rows->Set<uint32_t>("Event", header.GetEventID());
            rows->Set<uint32_t>("SubEvent", header.GetSubEventID());
            rows->SetEnumsAreInts(true); /* Disconnect seatbelt */	
            rows->Set<int32_t>("SubEventStream", streams_[header.GetSubEventStream()]);
            rows->Set<bool>("exists", false); // to be overwritten by someone else, e.g. I3TableWriter
            return 1;
        }

I3_CONVERTER(I3IndexColumnsGenerator, I3IndexColumnsGenerator);

