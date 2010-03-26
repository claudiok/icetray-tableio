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

#ifndef	I3CONVERTER_H_INCLUDED
#define I3CONVERTER_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

#include "icetray/IcetrayFwd.h"

#include "tableio/internals/I3TableRowDescription.h"
#include "tableio/internals/I3TableRow.h"

I3_FORWARD_DECLARATION(I3TableWriter);
I3_FORWARD_DECLARATION(I3FrameObject);

/******************************************************************************/

// basic interface to the individual converters. For this class pointer typdefs 
// will be created. All methods take unspecialized pointers to I3FrameObjects
class I3Converter {
    public:
        /**
         * Allows to show an I3FrameObject to this converter to get the number
         * of rows that this converter will produce
         */
        virtual size_t GetNumberOfRows(I3FrameObjectConstPtr object)=0;
        virtual size_t GetNumberOfRows(const I3FrameObject& object)=0;

        /**
         * return a I3TableRowDescription that specifies which table columns this converter
         * will fill. It may be useful to have some lazy initialization for the description:
         * wait for the first object to give the converter a chance
         * to adjust the description to this object
         * (e.g. think about fixed length map<string, double> where each
         * entry should become a column)
         *
         */
        virtual I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object) = 0;
        virtual I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object) = 0;

        /**
         * return a pointer to an already created description object. log_fatal if this
         * doesn't exist
         */
        I3TableRowDescriptionConstPtr GetDescription();
        bool HasDescription() { if (description_) return true; else return false; };
        
        /**
         * Fill the object held by pointer object into rows
         * to allow more complex converters a pointer to the current frame can 
         * be passed along
         * returns the number of rows it wrote
         */
        virtual size_t Convert(I3FrameObjectConstPtr object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr()) = 0;
        
        virtual size_t Convert(const I3FrameObject& object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr()) = 0;

        /**
         * return true if this converter can handle the object
         */
        virtual bool CanConvert(I3FrameObjectConstPtr object) = 0;

        
    protected:
        I3TableRowDescriptionPtr description_;
        I3FramePtr currentFrame_;

};

I3_POINTER_TYPEDEFS( I3Converter );

/******************************************************************************/

// base class for the individual converter implementations
// the pointer conversion from I3FrameObject to the respective type of the
// converter is handled here. Also it somewhat hides the passed I3FramePtr.
// So the majority of the converters can follow the scheme of converting
// only a single object and more complex converters can be build that
// access currentFrame_ to get what they need. 
// TODO: do converters need access to the context (e.g. services) ?

template<class FrmObj>
class I3ConverterImplementation : public I3Converter {
    public:
        
        typedef FrmObj booked_type;
        
        size_t GetNumberOfRows(I3FrameObjectConstPtr object) {
            return GetNumberOfRows(dynamic_cast<const FrmObj&>(*object));
        }
        
        size_t GetNumberOfRows(const I3FrameObject& object) {
            return GetNumberOfRows(dynamic_cast<const FrmObj&>(object));
        }
        
        size_t Convert(const I3FrameObject& object, I3TableRowPtr rows, 
                             I3FramePtr frame) {
            const FrmObj& typedObject = dynamic_cast<const FrmObj&>(object);

            // lazy initialization of the description:
            if (!description_)
               description_ = CreateDescription(typedObject);

            currentFrame_ = frame;

            size_t nrows = FillRows( typedObject, rows );
            currentFrame_.reset();
            return nrows;
        }
        
        size_t Convert(I3FrameObjectConstPtr object, I3TableRowPtr rows, I3FramePtr frame) {
            return Convert( *object, rows, frame);
        }

        I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object) {
            if (description_)
                return description_;
            else {
                description_ = CreateDescription(dynamic_cast<const FrmObj&>(object)); 
                return description_;
            }
        }

        I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object) {
            return GetDescription(*object);
        }

        virtual bool CanConvert(I3FrameObjectConstPtr object);


    protected:
        virtual I3TableRowDescriptionPtr CreateDescription(const FrmObj& object) = 0;
        virtual size_t GetNumberOfRows(const FrmObj& object);
        virtual size_t FillRows(const FrmObj& object, I3TableRowPtr rows) = 0;
};

/******************************************************************************/

// provide a default implemenation for GetNumberOfRows. covers all one-row-per-object converters.
template <class FrmObj>
size_t I3ConverterImplementation<FrmObj>::GetNumberOfRows(const FrmObj& object) {
    return 1;
}

/******************************************************************************/

// a default implementation of CanConvert. simply compares the template type to the pointer
template <class FrmObj>
bool I3ConverterImplementation<FrmObj>::CanConvert(I3FrameObjectConstPtr object) {
      return (typeid(FrmObj) == typeid(*object.get()));
}

/******************************************************************************/

// The unsigned integer type to use when writing size_t out
// to a table. This should be the largest pointer size of any platform
// on which IceTray runs (64 bits for the forseeable future).
typedef uint64_t tableio_size_t;

#endif
