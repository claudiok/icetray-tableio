#include <hdf-writer/internals/I3Converter.h>
#include <boost/python.hpp>
#include <icetray/I3FrameObject.h>

struct PythonConverter : I3Converter, boost::python::wrapper<I3Converter>
{
	// PythonConverter(PyObject* self) : self_(self) {};
	PythonConverter() {};
	
	
	// void Configure( const StringPairVector& params);
    
    unsigned int GetNumberOfRows(I3FrameObjectConstPtr object);

    I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object);

    I3TableRowDescriptionConstPtr GetDescription();

    // I3TableRowDescriptionPtr CreateDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionPtr CreateDescription(const I3FrameObject& object);
    
    unsigned int Convert(I3FrameObjectConstPtr object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());
    
    unsigned int Convert(const I3FrameObject& object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());

    unsigned int FillRows(const I3FrameObjectPtr object, I3TableRowPtr rows);

    bool CanConvert(I3FrameObjectConstPtr object);
    bool CanConvert(I3FrameObjectPtr object);


	PyObject* self_;
};