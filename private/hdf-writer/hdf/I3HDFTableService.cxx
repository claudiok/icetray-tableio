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


#include "hdf-writer/hdf/I3HDFTableService.h"
#include "hdf-writer/hdf/I3HDFTable.h"
//#include "hdf-writer/internals/I3TableRowDescription.h"
//#include "hdf-writer/internals/I3Table.h"

/******************************************************************************/

I3HDFTableService::I3HDFTableService(const std::string& filename, int compress) :
    I3TableService(),
    filename_(filename),
    compress_(compress)
    {

    fileId_ =  H5Fcreate(filename_.c_str(),
                         H5F_ACC_TRUNC, // truncate file if it exits
                         H5P_DEFAULT,   // default meta data creation 
                                        // - standard setting
                         H5P_DEFAULT);  // file access property 
                                        // - standard setting
}

/******************************************************************************/

I3HDFTableService::~I3HDFTableService() {}

/******************************************************************************/


I3TablePtr I3HDFTableService::CreateTable(const std::string& tableName, 
                              I3TableRowDescriptionConstPtr description) {
        
    I3TablePtr table(new I3HDFTable(*this, tableName, 
                                    description, fileId_, compress_));
    return table;
};
