/**
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Martin Wolf <martin.wolf@fysik.su.se> $LastChangedBy$
 */

#ifndef TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED
#define TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/I3Direction.h"

class I3DirectionConverter : public I3ConverterImplementation<I3Direction>
{
    public:
        enum BookRefFrame { car = 0, sph = 1, all = 2 };
    
        I3DirectionConverter()
        : book_ref_frame_(I3DirectionConverter::car)
        {}
        
        /** The book_ref_frame argument specifies the reference frame for which
         *  direction data should be booked.
         */
        I3DirectionConverter(BookRefFrame book_ref_frame)
        : book_ref_frame_(book_ref_frame)
        {}
    
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3Direction& direction);
        size_t FillRows(const I3Direction& direction, I3TableRowPtr row);
        
        BookRefFrame book_ref_frame_;
};

#endif // TABLEIO_I3DIRECTIONCONVERTER_H_INCLUDED
