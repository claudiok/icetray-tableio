/**
 * I3TableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
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
      ("I3HDFTableService", bp::init<const std::string>(bp::args("filename")))
      .def(bp::init<const std::string&,int>(bp::args("filename","compression_level")))
		;
}
