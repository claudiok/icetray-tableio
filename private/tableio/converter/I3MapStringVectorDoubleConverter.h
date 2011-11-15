/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3MapStringVectorDoubleConverter.h 64296 2010-05-25 16:13:48Z jvansanten $
 *
 * @version $Revision: 64296 $
 * @date $LastChangedDate: 2010-05-25 18:13:48 +0200 (Tue, 25 May 2010) $
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: jvansanten $
 */

#ifndef TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED
#define TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/I3Map.h"

class I3MapStringVectorDoubleConverter : public I3ConverterImplementation<I3MapStringVectorDouble> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3MapStringVectorDouble& map);
    size_t FillRows(const I3MapStringVectorDouble& map, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3MapStringVectorDouble& map);
};

#endif // TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED
