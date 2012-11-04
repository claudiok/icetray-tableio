/**
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de>
 * @author Jakob van Santen <vansanten@wisc.edu>
 Last changed by: $LastChangedBy$
 */

#ifndef TABLEIO_I3TREECONVERTER_H_INCLUDED
#define TABLEIO_I3TREECONVERTER_H_INCLUDED

#include <tableio/converter/container_converter_detail.h>
#include <dataclasses/I3Tree.h>
	 
template <class Converter, typename Container = I3Tree<typename Converter::booked_type> >
class I3TreeConverter : public I3ConverterImplementation<Container> {
private:
	typedef typename Converter::booked_type Element;
	Converter converter_;
protected:

	size_t
	GetNumberOfRows(const Container& c) {
		return c.size();
	}
	
	I3TableRowDescriptionPtr
	CreateDescription(const Container &c)
	{
		I3TableRowDescriptionPtr desc(new I3TableRowDescription());
		desc->isMultiRow_ = true;
		desc->AddField<tableio_size_t>("depth", "", "depth in tree");
		
		if (c.size())
			detail::add_fields(converter_, desc, *c.begin());
		else
			detail::add_fields(converter_, desc, Element());
		
		return desc;	
	}
	
	size_t FillRows(const Container &c, I3TableRowPtr rows)
	{
		size_t nrows = rows->GetNumberOfRows();
		assert( (nrows - rows->GetCurrentRow()) >= size_t(c.size()));
		
                size_t currentRow = rows->GetCurrentRow();

                size_t i_row = currentRow;
                size_t start_row = currentRow;
		for (typename Container::iterator it = c.begin(); it != c.end(); it++, i_row++) {
			rows->SetCurrentRow(i_row);
			rows->Set<tableio_size_t>("depth", c.depth(it));
			detail::fill_single_row(converter_, *it, rows, this->currentFrame_);
		}
		
                // check that nobody changed the row size
                assert(nrows == rows->GetNumberOfRows());
		
		return (i_row - start_row);
	}
};

#endif /* TABLEIO_I3TREECONVERTER_H_INCLUDED */