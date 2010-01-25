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

#include "hdf-writer/internals/I3TableService.h"
#include "hdf-writer/internals/I3TableRow.h"

I3TableService::I3TableService()  { }
        
I3TablePtr I3TableService::GetTable(std::string name,
                                    I3TableRowDescriptionConstPtr description) {
    std::map<std::string, I3TablePtr>::iterator it;
    I3TablePtr table;
    it = tables_.find(name);
    if (it != tables_.end()) {
        // test description 
        // TODO useful here? rethink task definitions
        if (description->CanBeFilledInto( it->second->GetDescription() ) )
            table = it->second;
    }

    return table;
}

/* partly moved into I3Table and GetTable
void I3TableService::AddRow(std::string tableName, I3TableRowConstPtr row) {
    I3TablePtr table = GetTable(tableName);

    // if the table doesn't exist create it
    if (!table) {
        table = CreateTable(row->GetDescription());
        tables_[tableName] = table;
    }

    // FIXME require identity or compatibility here?
    if (table->GetDescription() != row->GetDescription()) {
        log_fatal("I3TableRowDescription instances differ for table " 
                  "%s and the given row!", tableName.c_str()); 
    }

    // assure allignment
    if (!table->IsAligned())
        table->Align(eventHeaderCache_);
    
    table->AddRow(row);
    
    // implemenation pending
}
*/
        
I3TablePtr I3TableService::CreateTable(std::string tableName,
                                       I3TableRowDescriptionConstPtr description) {
    // implementation pending
    return I3TablePtr();
}

void I3TableService::Finish() {
    // implemenation pending
}

