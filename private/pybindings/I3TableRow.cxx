#include <hdf-writer/internals/I3TableRow.h>

// using namespace boost::python;
namespace bp = boost::python;

// TODO: proxy function with type-switching magic

void register_I3TableRow() {
	
	bp::class_<I3TableRow,
		boost::shared_ptr<I3TableRow> >
		("I3TableRow",
		bp::init<I3TableRowDescriptionConstPtr,unsigned int>())
	#define PROPS (CurrentRow)
	BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRow,PROPS)
	;
}
