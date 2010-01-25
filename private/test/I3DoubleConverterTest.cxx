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

#include "I3Test.h"

#include "dataclasses/I3Double.h"

#include "hdf-writer/internals/I3TableService.h"
#include "hdf-writer/internals/I3TableWriter.h"
#include "hdf-writer/internals/I3ConverterFactory.h"


TEST_GROUP(I3DoubleConverterTests);

TEST(factory_creation) {
    I3ConverterPtr converter = BuildConverter("I3Double");
    ENSURE(converter != 0, "factory didn't return a null pointer");

    // every converter has the chance to see the frame object
    // it is responsible for before returning the description.
    // hence a call to GetDescription() should now fail
    bool thrown = false;
    try {
        converter->GetDescription();
    }
    catch(...) {
        thrown = true;
    }
    ENSURE(thrown, "description not set yet");
}

TEST(factory_creation_no_parameter_check) {
    StringPairVector params;
    params.push_back(std::pair<std::string, std::string>("param1", "value1"));
    bool thrown = false;
    I3ConverterPtr converter;
    try {
        converter = BuildConverter("I3Double", params);
    }
    catch(...) {
        thrown = true;
    }

    ENSURE(!converter, "factory didn't produce anything");
    ENSURE(thrown, "call to log_fatal in not overridden Configure(params)");
}

TEST(convert_one_row) {
    I3ConverterPtr converter = BuildConverter("I3Double");

    I3DoubleConstPtr x = I3DoubleConstPtr( new I3Double(3.14) );
    I3FrameObjectConstPtr frameobj = x; 

    // create empty row
    // use the I3Double in x to initialize the description
    ENSURE( converter->GetDescription(x), "get description");
    I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(converter->GetDescription()));

    // inspect empty row
    ENSURE_EQUAL(rows->GetNumberOfRows(), static_cast<unsigned int>(1), "one row created");
    ENSURE_EQUAL(rows->GetDescription(), converter->GetDescription(), "description created and equal");
    ENSURE_EQUAL(rows->GetDescription()->GetNumberOfFields(), static_cast<unsigned int>(1), "description ok");
    ENSURE_EQUAL(rows->GetDescription()->GetFieldNames().at(0), "value", "description ok");
    ENSURE_EQUAL( rows->Get<double>("value"), 0, "rows are initialized to zero");
    
    // convert
    converter->Convert(frameobj, rows);
    
    // check
    ENSURE_DISTANCE( rows->Get<double>("value"), x->value, 1e-6, "I3Double value transferred to row");
}

TEST(convert_into_many_rows) {
    I3ConverterPtr converter = BuildConverter("I3Double");

    I3DoublePtr x = I3DoublePtr( new I3Double(3.14) );
    I3FrameObjectPtr frameobj = x; 

    // create 10 empty rows
    // use the I3Double in x to initialize the description
    ENSURE( converter->GetDescription(x), "get description");
    I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(converter->GetDescription(), 10));

    // inspect empty row
    ENSURE_EQUAL(rows->GetNumberOfRows(), static_cast<unsigned int>(10), "one row created");
    ENSURE_EQUAL(rows->GetDescription(), converter->GetDescription(), "description created and equal");
    ENSURE_EQUAL(rows->GetDescription()->GetNumberOfFields(), static_cast<unsigned int>(1), "description ok");
    ENSURE_EQUAL(rows->GetDescription()->GetFieldNames().at(0), "value", "description ok");
    for (int i = 0; i < 10; i++) {
        rows->SetCurrentRow(i);
        ENSURE_EQUAL( rows->Get<double>("value"), 0, "rows are initialized to zero");
        // convert
        x->value = 3.0 * i + 2;
        converter->Convert(frameobj, rows);
    }
    
    // check
    for (int i = 0; i < 10; i++) {
        rows->SetCurrentRow(i);
        ENSURE_DISTANCE( rows->Get<double>("value"), 3.0 * i + 2, 1e-6, "I3Double value transferred to row");

    }
}
