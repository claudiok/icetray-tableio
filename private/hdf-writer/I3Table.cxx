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

#include "hdf-writer/internals/I3Table.h"
#include "hdf-writer/internals/I3TableRow.h"
#include "hdf-writer/internals/I3TableRowDescription.h"

I3Table::I3Table(const I3TableService& service, 
                 std::string name,
                 I3TableRowDescriptionConstPtr description, 
                 unsigned int initialRows) :
    service_(service),
    name_(name),
    description_(description),
    indexTable_(),
    nevents_(0),
    nrows_(0) {
    // implementation pending
}

I3Table::~I3Table() {}; // Flush?

void I3Table::Create() {
    // implementation pending
}
void I3Table::Align() {
    // implementation pending
}

bool I3Table::IsAligned() {
    // implementation pending
}

bool I3Table::IsConnectedToWriter() {
    return connected_;
}
void I3Table::SetConnectedToWriter(bool connected) {
    connected_ = connected;
}
        
I3TableRowDescriptionConstPtr I3Table::GetDescription() {
    return description_;
}

void I3Table::AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row) {
    
}

unsigned int I3Table::GetNumberOfEvents() const {
   return nevents_; 
}

unsigned int I3Table::GetNumberOfRows() const {
    return nrows_;
}

std::string I3Table::GetName() const {
    return name_;
}
        
I3TableRowPtr I3Table::CreateRow(unsigned int nrows) {
    return I3TableRowPtr( new I3TableRow(description_, nrows) );
}
