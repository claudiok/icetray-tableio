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

#include <tableio/internals/I3TableService.h>

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

}
