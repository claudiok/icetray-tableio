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

#include <tableio/internals/I3Converter.h>

// I3ConverterBundlerepresents a bundle of converters all acting on the same frame object
// this can be used to extend the functionality of compiled bookers in a 
// more user-friendly way than with multiple inheritance
class I3ConverterBundle : public I3Converter {
    public:
        I3ConverterBundle(std::vector<I3ConverterPtr>& converters);
        // virtual ~I3ConverterBundle();
        virtual unsigned int GetNumberOfRows(I3FrameObjectConstPtr object);
        virtual I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object);
        virtual I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object);
        virtual unsigned int Convert(I3FrameObjectConstPtr object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr());
        
        virtual unsigned int Convert(const I3FrameObject& object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr());

        virtual bool CanConvert(I3FrameObjectConstPtr object);
    private:
        I3ConverterBundle();
        std::vector<I3ConverterPtr> converters_;
};
