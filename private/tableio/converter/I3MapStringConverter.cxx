/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/converter/I3MapStringConverter.h"
#include "tableio/I3ConverterFactory.h"
#include "boost/foreach.hpp"

I3TableRowDescriptionPtr I3MapStringDoubleConverter::CreateDescription(const I3MapStringDouble& map) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    BOOST_FOREACH( I3MapStringDouble::value_type pair, map ) {
        desc->AddField<double>(pair.first,"","");
    }
    return desc;
};

size_t I3MapStringDoubleConverter::FillRows(const I3MapStringDouble& map, I3TableRowPtr rows) {
    BOOST_FOREACH( I3MapStringDouble::value_type pair, map ) {
        rows->Set<double>(pair.first,pair.second);
    }
    return 1;
};

I3_CONVERTER(I3MapStringDoubleConverter, I3MapStringDouble);
