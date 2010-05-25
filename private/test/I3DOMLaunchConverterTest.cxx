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

#include <I3Test.h>

#include "tableio/I3Converter.h"
#include "tableio/I3ConverterFactory.h"

#include "dataclasses/physics/I3DOMLaunch.h"


TEST_GROUP(I3DOMLaunchConverterTests);

TEST(factory_creation) {
    I3ConverterPtr converter = BuildConverter("I3DOMLaunchSeriesMap");
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

TEST(description) {
    I3ConverterPtr converter = BuildConverter("I3DOMLaunchSeriesMap");
    
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(0), "Nothing in the map");
    
    OMKey omk(35,35);
    (*dlsm)[omk] = std::vector<I3DOMLaunch>();
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(0), "Still nothing in the map");
    
    I3DOMLaunch dl;
    (*dlsm)[omk].push_back(dl);
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(1), "Now there's one element in the map");
    
    I3TableRowDescriptionConstPtr desc = converter->GetDescription(dlsm);
    ENSURE( desc != 0, "converter returned non-null pointer.");
    
}

TEST(type_id) {
    I3ConverterPtr converter = BuildConverter("I3DOMLaunchSeriesMap");
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    ENSURE( converter->CanConvert(dlsm), "Converter recognizes its own.");
}

TEST(assignment) {
    I3ConverterPtr converter = BuildConverter("I3DOMLaunchSeriesMap");
    
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    
    OMKey omk(35,35);
    std::vector<I3DOMLaunch> dls;
    I3DOMLaunch dl;
    dl.SetStartTime(42.0);
    dl.SetTriggerType(I3DOMLaunch::ONBOARD_LED);
    dl.GetRawATWD(0).assign(128,42);
    dls.push_back(dl);
    (*dlsm)[omk] = dls;

    I3TableRowDescriptionConstPtr desc = converter->GetDescription(dlsm);
    I3TableRowPtr rows(new I3TableRow(desc,converter->GetNumberOfRows(dlsm)));
    converter->Convert(dlsm, rows);
    
    ENSURE_EQUAL( rows->Get<double>("start_time"), 42.0);
    ENSURE_EQUAL( rows->Get<I3DOMLaunch::TriggerType>("trigger_type"), I3DOMLaunch::ONBOARD_LED);
    
    size_t index = desc->GetFieldColumn("raw_atwd_0");
    ENSURE_EQUAL( dl.GetRawATWD(0).size(), desc->GetFieldArrayLengths().at(index), "Array is the same length as the vector");
    
    std::vector<int>::const_iterator it;
    uint16_t* pointy = rows->GetPointer<uint16_t>("raw_atwd_0");
    size_t i;
    for (it = dl.GetRawATWD(0).begin(), i = 0; it != dl.GetRawATWD(0).end(); it++, i++) {
        ENSURE_EQUAL( pointy[i], *it, "Vector and array contents match.");
    }

}


