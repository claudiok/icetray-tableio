.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

Writing an I3Converter in C++
**********************************

.. highlight:: c++

You can also define your converters in C++. To do this, you need to inherit
from the templated C++ class I3ConverterImplementation<YourFrameObject>. The mechanics of adding the converter to your project are more convoluted The following will demonstrate how this in done for I3FilterResultMap from jebclasses.

CMakeLists.txt
___________________

.. highlight:: none

To use compiled converters, your project must link against tableio. You can
use the following conditional-compilation pattern in your CMakeLists file to
avoid a hard dependency on the tableio project::

    i3_project(jebclasses
      DOCS_DIR resources/docs)

    # define the lists used to build the basic library
    SET(LIB_${PROJECT_NAME}_SOURCEFILES private/jebclasses/*.cxx private/jebclasses/*/*.cxx)
    SET(LIB_${PROJECT_NAME}_PROJECTS icetray dataclasses)
    SET(LIB_${PROJECT_NAME}_TOOLS log4cplus boost python)

    # add some things if hdf-writer is installed
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/tableio)
        LIST(APPEND LIB_${PROJECT_NAME}_PROJECTS tableio)
        LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES private/converter/*.cxx)
        ADD_DEFINITIONS(-DUSE_TABLEIO)
        MESSAGE(STATUS "     + tableio support")
    endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/tableio)

    # finally, define the library
    i3_add_library(${PROJECT_NAME} ${LIB_${PROJECT_NAME}_SOURCEFILES}
      USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
      USE_PROJECTS ${LIB_${PROJECT_NAME}_PROJECTS}
    )

The source files and dependencies related to tableio are only added if the directory build/tableio exists.

.. highlight:: c++

public/jebclasses/converter/I3FilterResultMap.h
__________________________________________________

You can declare your subclass in a header, like so::

    #include "tableio/internals/I3ConverterFactory.h"
    #include "jebclasses/I3FilterResult.h"
    
    class I3FilterResultMapConverter : public I3ConverterImplementation<I3FilterResultMap> {
        private:
            I3TableRowDescriptionPtr CreateDescription(const I3FilterResultMap& frmap);
            unsigned int FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows);
            bool magic_;
    };

private/converter/I3FilterResultMap.cxx
__________________________________________

The implementation goes in a separate file::

    #include "jebclasses/converter/I3FilterResultMapConverter.h"

    I3TableRowDescriptionPtr I3FilterResultMapConverter::CreateDescription(const I3FilterResultMap& frmap) {
        I3TableRowDescriptionPtr desc = 
            I3TableRowDescriptionPtr(new I3TableRowDescription() );
        I3FilterResultMap::const_iterator it;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            desc->AddField<bool>(it->first,"bool","Field 0: condition passed, Field 1: prescale passed",2);
        }
        return desc;
    }
        
    unsigned int I3FilterResultMapConverter::FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows) {
        I3FilterResultMap::const_iterator it;
        bool* filter_result;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            filter_result = rows->GetPointer<bool>(it->first);
            filter_result[0] = it->second.conditionPassed;
            filter_result[1] = it->second.prescalePassed;
        }
        return 1;
    }


pybindings
__________________________________________

You also need trivial pybindings in order to register your converter for use
in tableio. There are preprocessor macros that automate most of this. You can
wrap the whole export block in #ifdefs to avoid hard dependencies::

    #ifdef USE_TABLEIO
    
    #include "jebclasses/converter/I3FilterResultMapConverter.h"
    #include "tableio/converter/pybindings.h"
    
    void register_I3Converters() {
        I3CONVERTER_NAMESPACE(jebclasses);
        I3CONVERTER_EXPORT(I3FilterResultMapConverter,"Dumps the result of each Pole filter to a table column");
    }
    
    #else
    void register_I3Converters() {}
    #endif

The macro I3CONVERTER_NAMESPACE(jebclasses) sets up a Python module
icecube.jebclasses.converters into which pybindings for
I3FilterResultMapConverter are exported. Each :cfunc:`I3CONVERTER_EXPORT` call
creates pybindings for the specified converter and registers it for use in
tableio.
