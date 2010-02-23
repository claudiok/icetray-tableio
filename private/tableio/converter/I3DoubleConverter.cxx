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
#include "tableio/internals/I3ConverterFactory.h"
#include "tableio/converter/I3STLContainerConverter.h"
#include "dataclasses/I3Double.h"

class I3DoubleConverter : public I3ConverterImplementation<I3Double> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3Double& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc->AddField<double>("value", "unit", "doc");
            return desc;
        }

        size_t FillRows(const I3Double& object, I3TableRowPtr rows) {
            rows->Set<double>("value", object.value);
            return 1;
        }
};

I3_CONVERTER(I3DoubleConverter, I3Double);
I3_CONVERTER(I3STLVectorConverter<I3Double>, std::vector<I3Double>);


