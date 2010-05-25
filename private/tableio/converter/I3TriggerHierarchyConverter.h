/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_I3TRIGGERHIERARCHYCONVERTER_H_INCLUDED
#define TABLEIO_I3TRIGGERHIERARCHYCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"

class I3TriggerHierarchyConverter : public I3ConverterImplementation<I3TriggerHierarchy> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TriggerHierarchy& triggers);
    size_t FillRows(const I3TriggerHierarchy& triggers, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3TriggerHierarchy& triggers);
};

#endif // TABLEIO_I3TRIGGERHIERARCHYCONVERTER_H_INCLUDED
