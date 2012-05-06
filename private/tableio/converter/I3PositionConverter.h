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

#ifndef TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
#define TABLEIO_I3POSITIONCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/I3Position.h"

class I3PositionConverter : public I3ConverterImplementation<I3Position>
{
    public:
        enum BookRefFrame { car = 0, sph = 1, cyl = 2, all = 3 };
        
        I3PositionConverter()
        : book_ref_frame_(I3PositionConverter::car)
        {}
        
        /** The book_ref_frame argument specifies the reference frame for which
         *  position data should be booked.
         */
        I3PositionConverter(BookRefFrame book_ref_frame)
        : book_ref_frame_(book_ref_frame)
        {}
        
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3Position& position);
        size_t FillRows(const I3Position& position, I3TableRowPtr row);

        BookRefFrame book_ref_frame_;
};

#endif // TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
