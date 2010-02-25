#include <tableio/internals/I3Converter.h>
#include <boost/python.hpp>
#include <icetray/I3FrameObject.h>

struct PythonConverter : I3Converter, boost::python::wrapper<I3Converter>
{

    PythonConverter() {};

    // void Configure( const StringPairVector& params);

    size_t GetNumberOfRows(I3FrameObjectConstPtr object);

    I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object);

    I3TableRowDescriptionConstPtr GetDescription();

    I3TableRowDescriptionPtr CreateDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionPtr CreateDescription(const I3FrameObject& object);
    
    size_t Convert(I3FrameObjectConstPtr object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());
    
    size_t Convert(const I3FrameObject& object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());

    size_t FillRows(const I3FrameObjectConstPtr object, I3TableRowPtr rows);

    bool CanConvert(I3FrameObjectConstPtr object);
    bool CanConvert(I3FrameObjectPtr object);

};

