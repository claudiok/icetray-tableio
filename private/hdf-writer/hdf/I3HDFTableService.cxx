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

/******************************************************************************/

I3HDFTableService::I3HDFTableService(const std::string& filename, int compress, char mode) :
    I3TableService(),
    filename_(filename),
    compress_(compress)
    // tables_()
    {
    if ( mode == 'w') { 
    fileId_ =  H5Fcreate(filename_.c_str(),
                         H5F_ACC_TRUNC, // truncate file if it exits
                         H5P_DEFAULT,   // default meta data creation 
                                        // - standard setting
                         H5P_DEFAULT);  // file access property 
                                        // - standard setting
   } else if ( mode == 'r' ) {
      fileId_ = H5Fopen(filename_.c_str(),
                        H5F_ACC_RDONLY,
                        H5P_DEFAULT );
      if (fileId_ < 0) {
         log_fatal("Could not open HDF file '%s'",filename_.c_str());
      }
      FindTables();
   } else {
      log_fatal("Unsupported file mode '%c'",mode);
   }
}

/******************************************************************************/

// A callback for browsing HDF nodes
herr_t GatherTableNames(hid_t group_id, const char* member_name, void* target) {
   reinterpret_cast<std::vector<std::string>*>(target)->push_back(std::string(member_name));
   return 0;
}

/******************************************************************************/

// Gather tables hanging from the root note
void I3HDFTableService::FindTables() {

   // get my pretties from the root group
   std::vector<std::string> tableNames;
   H5Giterate(fileId_,"/",NULL,&GatherTableNames,&tableNames); 
   std::vector<std::string>::iterator t_it;
   std::string name;
   
   for (t_it = tableNames.begin(); t_it != tableNames.end();) {
      hsize_t nfields,nrecords;
      name = "";
      name += *t_it;
      if (H5TBget_table_info(fileId_,name.c_str(),&nfields,&nrecords) < 0) {
         // erase returns iterator to following element
         t_it = tableNames.erase(t_it);
         log_error("Whups! Couldn't read dataset at %s",name.c_str());
         continue;
      }  
      
      // it's a table, read in the description
      tables_[*t_it] = I3TablePtr(new I3HDFTable(*this,*t_it, fileId_));
      
      t_it++;
      
   }
}

/******************************************************************************/

I3HDFTableService::~I3HDFTableService() {};

/******************************************************************************/


I3TablePtr I3HDFTableService::CreateTable(const std::string& tableName, 
                              I3TableRowDescriptionConstPtr description) {
        
    I3TablePtr table(new I3HDFTable(*this, tableName, 
                                    description, fileId_, compress_));
    return table;
};

void I3HDFTableService::CloseFile() {
	log_warn("Closing '%s'. Did I want to do some sanity checks first?",filename_.c_str());
	H5Fclose(fileId_);
};
