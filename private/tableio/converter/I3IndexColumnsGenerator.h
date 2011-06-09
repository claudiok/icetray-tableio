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

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/physics/I3EventHeader.h"

class I3IndexColumnsGenerator : public I3ConverterImplementation<I3EventHeader> {
    public:
	I3IndexColumnsGenerator() {};
	I3IndexColumnsGenerator(const std::vector<std::string> &streams);
	I3IndexColumnsGenerator(I3TableRowDescriptionConstPtr desc);
    private:
	I3TableRowDescriptionPtr CreateDescription(const I3EventHeader& object);
	size_t FillRows(const I3EventHeader& header, I3TableRowPtr rows);
	
	/* TODO: some sort of hashing to make this less horrible */
	typedef std::map<std::string,int> stream_map_t;
	typedef std::vector<stream_map_t::key_type> istream_t;
	stream_map_t streams_;
	istream_t istreams_;
    public:
	I3EventHeaderPtr Resurrect(I3TableRowPtr rows);
};
