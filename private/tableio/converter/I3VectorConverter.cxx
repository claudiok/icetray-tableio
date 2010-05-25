/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/I3ConverterFactory.h"
#include "tableio/converter/I3VectorConverter.h"

#include <boost/preprocessor.hpp>

I3_CONVERTER(I3VectorConverter<bool>, I3Vector<bool>);

I3_CONVERTER(I3VectorConverter<int16_t>, I3Vector<int16_t>);
I3_CONVERTER(I3VectorConverter<uint16_t>, I3Vector<uint16_t>);

I3_CONVERTER(I3VectorConverter<int32_t>, I3Vector<int32_t>);
I3_CONVERTER(I3VectorConverter<uint32_t>, I3Vector<uint32_t>);

I3_CONVERTER(I3VectorConverter<I3VectorInt64::value_type>, I3Vector<I3VectorInt64::value_type>);
I3_CONVERTER(I3VectorConverter<I3VectorUInt64::value_type>, I3Vector<I3VectorUInt64::value_type>);

I3_CONVERTER(I3VectorConverter<float>, I3Vector<float>);
I3_CONVERTER(I3VectorConverter<double>, I3Vector<double>);

typedef I3VectorConverter< std::pair<double, double> > I3VectorDoubleDoubleConverter;
I3_CONVERTER(I3VectorDoubleDoubleConverter, I3VectorDoubleDouble);
