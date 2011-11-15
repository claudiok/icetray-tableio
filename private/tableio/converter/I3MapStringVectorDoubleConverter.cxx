/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision: 65667 $
 * @date $LastChangedDate: 2010-07-20 19:30:56 +0200 (Tue, 20 Jul 2010) $
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/converter/I3MapStringVectorDoubleConverter.h"
#include "tableio/I3ConverterFactory.h"
#include "boost/foreach.hpp"
#include <tableio/converter/container_converter_detail.h>

size_t I3MapStringVectorDoubleConverter::GetNumberOfRows(const I3MapStringVectorDouble& map) {
        I3MapStringVectorDouble::const_iterator mapiter = map.begin();
        size_t maxindex = mapiter->second.size();

        return maxindex;
    }

I3TableRowDescriptionPtr I3MapStringVectorDoubleConverter::CreateDescription(const I3MapStringVectorDouble& m) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->isMultiRow_ = true;
        desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
    BOOST_FOREACH( I3MapStringVectorDouble::value_type p, m) {
        desc->AddField<double>(p.first,"","");
    }
    if (m.size() == 0)
      log_error("Warning:  I3MapStringVectorDouble used for converter configuration is EMPTY, nothing will be booked");
    return desc;
};

size_t I3MapStringVectorDoubleConverter::FillRows(const I3MapStringVectorDouble& map, I3TableRowPtr rows) {
//    size_t maxindex = 0;
    I3MapStringVectorDouble::const_iterator mapiter = map.begin();
    size_t maxindex = mapiter->second.size();
//    rows->SetNumberOfRows(maxindex);

    for(mapiter = map.begin();
        mapiter != map.end();
        mapiter++)
    { 
//        maxindex = mapiter->second.size();
//        rows->SetNumberOfRows(maxindex);
        for (size_t i = 0; i < mapiter->second.size(); i++)
        {
            rows->SetCurrentRow(i);
            rows->Set<double>(mapiter->first,mapiter->second[i]);
        }
    }

    for(size_t i = 0; i < maxindex; i++)
    {
        rows->SetCurrentRow(i);
        //rows->Set<tableio_size_t>("vector_index",i);
        rows->Set<size_t>("vector_index",i);
    }
/*
   for(I3MapStringVectorDouble::const_iterator mapiter = map.begin();
	mapiter != map.end();
	mapiter++)
    {
        rows->SetCurrentRow(i);
        rows->Set<double>(mapiter->first)
    }
*/
    return maxindex;
};

I3_CONVERTER(I3MapStringVectorDoubleConverter, I3MapStringVectorDouble);
