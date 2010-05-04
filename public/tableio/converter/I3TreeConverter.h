/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_I3TREECONVERTER_H_INCLUDED
#define TABLEIO_I3TREECONVERTER_H_INCLUDED

#include "tableio/internals/I3ConverterFactory.h"
#include "dataclasses/physics/I3MCTree.h"

template<class FrmObj>
class I3TreeConverter : public I3ConverterImplementation<I3Tree<FrmObj> > {
    public:
        // TODO constructor, needs to get ItemConverter
        I3TreeConverter(I3ConverterPtr itemConverter) : 
            I3ConverterImplementation<I3Tree<FrmObj> >(),
            itemConverter_(itemConverter) 
        {}
        
        I3TreeConverter() : 
            I3ConverterImplementation<I3Tree<FrmObj> >(),
            itemConverter_(BuildConverter(I3::name_of<FrmObj>()))
        {}
        
    private:
        // member variables
        I3ConverterPtr itemConverter_;

        // member functions
        I3TableRowDescriptionPtr CreateDescription(const I3Tree<FrmObj>& tree) {
            I3TableRowDescriptionPtr desc_tree = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc_tree->isMultiRow_ = true;
            desc_tree->AddField<tableio_size_t>("depth", "", "depth in tree");

            I3TableRowDescriptionConstPtr desc_items;

            if (tree.size() > 0) {
                const FrmObj& obj = *(tree.begin());
                desc_items = itemConverter_->GetDescription(obj);
            }
            else {
                FrmObj obj; // FrmObj needs a default constructor
                desc_items = itemConverter_->GetDescription(obj);
            }
            I3TableRowDescriptionPtr desc;
            desc = I3TableRowDescriptionPtr(new I3TableRowDescription( *desc_tree | *desc_items ));
            return desc;
        }
        
        size_t FillRows(const I3Tree<FrmObj>& tree, I3TableRowPtr rows) {
            size_t nrows = rows->GetNumberOfRows();
            assert( (nrows - rows->GetCurrentRow()) >= static_cast<size_t>(tree.size()));

            typename I3Tree<FrmObj>::iterator it;
            size_t currentRow = rows->GetCurrentRow();

            size_t i_row = currentRow;
            size_t start_row = currentRow;
            for (it = tree.begin(); it != tree.end(); ++it) {
                rows->SetCurrentRow(i_row);
                tableio_size_t depth = tree.depth(it);
                
                // let itemConverter_ fill the current row (and maybe even more)
                // currentFrame_ is not in the scope of this function?
                // use this-> to access I3Converter::currentFrame_ TODO: test that this works correctly!
                unsigned rowsWritten = itemConverter_->Convert(*it, rows, this->currentFrame_);

                // for all rows the itemConverter has changed the vector_index
                // field must be set to current index
                for (size_t j_row = 0; j_row < rowsWritten; ++j_row) {
                    rows->SetCurrentRow(i_row+j_row);
                    rows->Set<tableio_size_t>("depth", depth);
                }

                i_row += rowsWritten;
            }

            // check that nobody changed the row size
            assert(nrows == rows->GetNumberOfRows());

            return i_row - start_row;
        }

        size_t GetNumberOfRows(const I3Tree<FrmObj>& tree) {
            typename I3Tree<FrmObj>::iterator it;
            size_t totalsize = 0;
            for(it = tree.begin(); it != tree.end(); ++it) 
                totalsize += itemConverter_->GetNumberOfRows(*it);

            return totalsize;
        }
};

#endif // TABLEIO_I3TREECONVERTER_H_INCLUDED
