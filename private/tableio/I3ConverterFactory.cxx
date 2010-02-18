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

#include "tableio/internals/I3ConverterFactory.h"

I3ConverterPtr BuildConverter(std::string name) {
    I3ConverterPtr converter = 
        I3::Singleton<I3ConverterFactory>::get_const_instance()
        .Create(name)();
    return converter;
}
