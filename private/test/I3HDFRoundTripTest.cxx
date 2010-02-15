/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TableRowTest.cxx 61494 2010-02-06 02:47:01Z jvansanten $
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy: jvansanten $
 */

#include <I3Test.h>
#include <limits.h>

#include "H5TA.h"

#include <hdf-writer/internals/I3TableRow.h>
#include <hdf-writer/internals/I3TableRowDescription.h>
#include <hdf-writer/hdf/I3HDFTableService.h>
#include <hdf-writer/hdf/I3HDFTable.h>

#include <dataclasses/physics/I3EventHeader.h>

TEST_GROUP(I3HDFRoundTripTests);

TEST(resurrection) {
   // set up a writer service
   I3TableServicePtr writer_service = I3TableServicePtr(new I3HDFTableService("foo.hd5"));

   // build up a data type
   I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
   desc->SetIsMultiRow(true); // we plan to write multiple rows
   desc->AddField<double>("double","double-units","A thing which is doubly precise, verily, the float is unworthy look upon.");
   desc->AddField<short>("shorty","short-units","Short is as short does. Unless things have to be aligned to 32 bits, that is.");
   desc->AddField<char>("charlatan","letter","Speak your piece, but in 8 bits or less.");
   desc->AddField<long>("long_dooby_doo_long_long","smoots",
         "The length of things which are long, measured in units of the height of one \
         Oliver R. Smoot. Not entirely coincidentally, this was also the unit used to \
         measure the length of the Harvard Bridge in October 1958 by rolling one \
         Oliver R. Smoot end-over-end from one side of the Charles River to the other.\
         Smoot chaired ANSI from 2001-2002 and was president of ISO from 2003-2004.\
         [http://en.wikipedia.org/wiki/Smoot]");
   
	bool thrown = false;
	try { desc->AddField<bool>("farcicality","truth-units","Has this test become a farce?"); }
	catch (...) { thrown = true; }
	ENSURE( thrown, "Setting a boolean field with a non-empty unit string that is not 'bool' throws an error.");
   
	desc->AddField<bool>("farcicality","","Has this test become a farce?");
	
    desc->AddField<int>("vector","","A vector of ints",256);
   
   
   // create a table in the HDF file
   I3TablePtr table = writer_service->GetTable("paso_double",desc);
   
   // get container, fill container, write container
   size_t nrows = 3;
   I3TableRowPtr rows = table->CreateRow(nrows);

   double doubleval = 3.14;
   short shortval = 512;
   short charval = CHAR_MAX;
   long longval = LONG_MAX;
   bool boolval = true;
   size_t i,j;
   
   for(i=0; i<nrows; i++) {
      rows->SetCurrentRow(i);
      rows->Set<double>("double",doubleval);
      rows->Set<short>("shorty",shortval);
      rows->Set<char>("charlatan",charval);
      rows->Set<long>("long_dooby_doo_long_long",longval);
      rows->Set<bool>("farcicality",boolval);
      int* vec = rows->GetPointer<int>("vector");
      for (j=0;j<256;j++) vec[j] = j;
   }

   I3EventHeaderConstPtr fake_header = I3EventHeaderConstPtr(new I3EventHeader());
   table->AddRow(fake_header,rows);
   
   // clean up
   writer_service->Finish();
   
   // reopen the file for reading
   I3TableServicePtr reader_service = I3TableServicePtr(new I3HDFTableService("foo.hd5",1,'r'));
   
   // convention: desc = NULL => only return table if it exists on disk
   I3TablePtr zombie_table = reader_service->GetTable("paso_double",I3TableRowDescriptionPtr());
   
   ENSURE( zombie_table != NULL, "The reader service gives us a non-null pointer");
   
   I3TableRowDescriptionConstPtr zombie_desc = zombie_table->GetDescription();
   
   // compare descriptions
   std::vector<std::string>::const_iterator os_it, zs_it;
   for (os_it  = desc->GetFieldNames().begin(), zs_it = zombie_desc->GetFieldNames().begin(), i=0;
        os_it != desc->GetFieldNames().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field names match");
   }
   for (os_it  = desc->GetFieldUnits().begin(), zs_it = zombie_desc->GetFieldUnits().begin(), i=0;
        os_it != desc->GetFieldUnits().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field units match");
   }
   for (os_it  = desc->GetFieldDocStrings().begin(), zs_it = zombie_desc->GetFieldDocStrings().begin(), i=0;
        os_it != desc->GetFieldDocStrings().end();
        os_it++, zs_it++, i++) {
            ENSURE_EQUAL( *os_it, *zs_it, "Field doc strings match");
   }
   
   std::vector<size_t>::const_iterator osize_it,zsize_it;
   for (osize_it  = desc->GetFieldTypeSizes().begin(), zsize_it = zombie_desc->GetFieldTypeSizes().begin(), i=0;
        osize_it != desc->GetFieldTypeSizes().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field type sizes match.");
   }
   for (osize_it  = desc->GetFieldByteOffsets().begin(), zsize_it = zombie_desc->GetFieldByteOffsets().begin(), i=0;
        osize_it != desc->GetFieldByteOffsets().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field byte offsets match");
   }
   for (osize_it  = desc->GetFieldChunkOffsets().begin(), zsize_it = zombie_desc->GetFieldChunkOffsets().begin(), i=0;
        osize_it != desc->GetFieldChunkOffsets().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field chunk offsets match");
   }
   for (osize_it  = desc->GetFieldArrayLengths().begin(), zsize_it = zombie_desc->GetFieldArrayLengths().begin(), i=0;
        osize_it != desc->GetFieldArrayLengths().end();
        osize_it++, zsize_it++, i++) {
            ENSURE_EQUAL( *osize_it, *zsize_it, "Field array offsets match");
   }
   
   std::vector<char>::const_iterator opytype_it,zpytype_it;
   for (opytype_it  = desc->GetFieldTypeCodes().begin(), zpytype_it = zombie_desc->GetFieldTypeCodes().begin(), i=0;
        opytype_it != desc->GetFieldTypeCodes().end();
        opytype_it++, zpytype_it++, i++) {
            std::string msg = "Python-style type codes match for field ";
            msg += desc->GetFieldNames().at(i);
            ENSURE_EQUAL( *opytype_it, *zpytype_it, msg.c_str());
   }
   
   std::vector<hid_t>::const_iterator ohid_it,zhid_it;
   for (ohid_it  = desc->GetFieldHdfTypes().begin(), zhid_it = zombie_desc->GetFieldHdfTypes().begin(), i=0;
        ohid_it != desc->GetFieldHdfTypes().end();
        ohid_it++, zhid_it++, i++) {
         // does HDF5 know about these data types?
         H5I_type_t t_orig   = H5Iget_type( *ohid_it );
         H5I_type_t t_zombie = H5Iget_type( *zhid_it );
      
         ENSURE_EQUAL( t_orig , H5I_DATATYPE,  "Original datatype exists and is valid" );
         ENSURE_EQUAL( t_zombie, H5I_DATATYPE, "Zombie datatype exists and is valid" );
         
         ENSURE( H5Tequal(*ohid_it, *zhid_it) , "Field HDF types match");
   }
   
   // ReadRows is public only for I3HDFTable
   I3TableRowConstPtr zombie_rows = ((I3HDFTable*)(zombie_table.get()))->ReadRows(0,nrows);
   
   I3TableRowPtr castaway = boost::const_pointer_cast<I3TableRow>(zombie_rows);
   
   for (i=0; i<nrows; i++) {
      castaway->SetCurrentRow(i);
      ENSURE_EQUAL( doubleval, castaway->Get<double>("double"), "Read double is equal to set double."); 
      ENSURE_EQUAL( longval, castaway->Get<long>("long_dooby_doo_long_long"), "Read long is equal to set long."); 
      ENSURE_EQUAL( shortval, castaway->Get<short>("shorty"), "Read short is equal to set short.");
      ENSURE_EQUAL( charval, castaway->Get<char>("charlatan"), "Read char is equal to set char.");
      ENSURE_EQUAL( boolval, castaway->Get<bool>("farcicality"), "Read bool is equal to set bool.");
      int* vec = castaway->GetPointer<int>("vector");
      for (j=0;j<256;j++) ENSURE_EQUAL( (int)j, vec[j], "Vector contents are the same");
   }

   size_t obuffer_size = desc->GetTotalByteSize();
   size_t zbuffer_size = zombie_desc->GetTotalByteSize();
   
   ENSURE( obuffer_size > 0, "There's actually something in the write buffer");
   
   ENSURE_EQUAL( obuffer_size, zbuffer_size, "Written and read rows have equal-sized buffers.");
   ENSURE( memcmp(rows->GetPointer(),zombie_rows->GetPointer(),obuffer_size*nrows) == 0, "Written and read buffers are equal, byte for byte.");

   reader_service->Finish();
}
