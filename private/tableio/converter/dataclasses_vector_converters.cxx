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
#include "dataclasses_vector_converters.h"

I3_CONVERTER(I3VectorConverter< convert::pod<bool> >, I3Vector<bool>);

I3_CONVERTER(I3VectorConverter< convert::pod<int16_t> >, I3Vector<int16_t>);
I3_CONVERTER(I3VectorConverter< convert::pod<uint16_t> >, I3Vector<uint16_t>);

I3_CONVERTER(I3VectorConverter< convert::pod<int32_t> >, I3Vector<int32_t>);
I3_CONVERTER(I3VectorConverter< convert::pod<uint32_t> >, I3Vector<uint32_t>);

I3_CONVERTER(I3VectorConverter< convert::pod<I3VectorInt64::value_type> >, I3Vector<I3VectorInt64::value_type>);
I3_CONVERTER(I3VectorConverter< convert::pod<I3VectorUInt64::value_type> >, I3Vector<I3VectorUInt64::value_type>);

I3_CONVERTER(I3VectorConverter< convert::pod<float> >, I3Vector<float>);
I3_CONVERTER(I3VectorConverter< convert::pod<double> >, I3Vector<double>);

typedef I3VectorConverter< convert::double_pair > I3VectorDoubleDoubleConverter;
I3_CONVERTER(I3VectorDoubleDoubleConverter, I3VectorDoubleDouble);

I3_CONVERTER(I3VectorConverter< convert::I3FlasherInfo >, I3FlasherInfoVect);
