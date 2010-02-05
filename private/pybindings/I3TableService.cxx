/**
 * I3TableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TableRowDescription.cxx 61336 2010-01-29 01:26:18Z jvansanten $
 *
 * @version $Revision: 61336 $
 * @date $LastChangedDate: 2010-01-28 19:26:18 -0600 (Thu, 28 Jan 2010) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
 */

#include <hdf-writer/hdf/I3HDFTableService.h>

namespace bp = boost::python;

class I3TableServiceWrapper : I3TableService, bp::wrapper<I3TableService> {
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description) {
        return this->get_override("CreateTable")(tableName,description);
			};
        virtual void CloseFile() {
			this->get_override("CloseFile")();
};
   
};

void register_I3TableService() {

	// expose the virtual base class
	bp::class_<I3TableService, boost::noncopyable>("I3TableService",bp::no_init);

   bp::class_<I3HDFTableService, 
      boost::shared_ptr<I3HDFTableService>, bp::bases<I3TableService> >
      ("I3HDFTableService", bp::init<const std::string&,int>(bp::args("filename","compression_level")))
		;
}
