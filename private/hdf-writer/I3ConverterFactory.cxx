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

#include "hdf-writer/internals/I3ConverterFactory.h"

I3ConverterPtr BuildConverter(std::string name, StringPairVector params) {
    I3ConverterPtr converter = 
        I3::Singleton<I3ConverterFactory>::get_const_instance()
        .Create(name)();
    converter->Configure(params);
    return converter;
}

I3ConverterPtr BuildConverter(std::string name) {
    StringPairVector params;
    return BuildConverter(name, params);
}

