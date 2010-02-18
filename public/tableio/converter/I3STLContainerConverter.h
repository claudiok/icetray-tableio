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

// the converters should be written to be indifferent on the state of the I3TableRow.
// So if an I3DOMLaunch converter exists, converters for vector<I3DOMLaunch> and
// map<omkey, vector<I3DOMLaunch>> could be obtained for free.
//
// FIXME: vector<I3DOMLaunch> is no I3FrameObject. how to get this through the I3Converter
//        interface?

#include "hdf-writer/internals/I3ConverterFactory.h"
#include "dataclasses/physics/I3EventHeader.h"

template <class FrmObj>
class I3STLVectorConverter : public I3ConverterImplementation<std::vector<FrmObj> > {
    private:
        unsigned int GetNumberOfRows(std::vector<FrmObj>& vect) {
            typename std::vector<FrmObj>::const_iterator it;
            unsigned int totalsize = 0;
            for(it = vect.begin(); it != vect.end(); ++it) 
                totalsize += itemConverter_->GetNumberOfRows(*it);

            return totalsize;
        }

        I3TableRowDescriptionPtr CreateDescription(const std::vector<FrmObj>& vect) {
            I3TableRowDescriptionPtr desc_vector = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc_vector->isMultiRow_ = true;
            desc_vector->AddField<unsigned int>("vector_index", "", "index in vector");

            // create the converter for the elements of the vector
            // FIXME: pass params?
            I3ConverterPtr conv;
            if (converterName_ != "") 
                itemConverter_ = BuildConverter(converterName_);
            else
                itemConverter_ = BuildConverter(I3::name_of<FrmObj>());

            //itemConverter_ = dynamic_pointer_cast<I3ConverterImplementation<FrmObj> >(conv);
            I3TableRowDescriptionConstPtr desc_items;

            if (vect.size() > 0) {
                const FrmObj& obj = vect.at(0);
                desc_items = itemConverter_->GetDescription(obj);
            }
            else {
                FrmObj obj; // need a default constructor
                desc_items = itemConverter_->GetDescription(obj);
            }
            I3TableRowDescriptionPtr desc;
            desc = I3TableRowDescriptionPtr(new I3TableRowDescription( *desc_vector | *desc_items ));
            return desc;
        }

        unsigned int FillRows(const std::vector<FrmObj>& vec, I3TableRowPtr rows) {
            unsigned int nrows = rows->GetNumberOfRows();
            assert( (nrows - rows->GetCurrentRow()) >= vec.size());

            typename std::vector<FrmObj>::const_iterator it;
            unsigned int currentRow = rows->GetCurrentRow();

            unsigned int v_index = 0;
            unsigned int i_row = currentRow;
            for (it = vec.begin(); it != vec.end(); ++it) {
                rows->SetCurrentRow(i_row);
                
                // let itemConverter_ fill the current row (and maybe even more)
                // currentFrame_ is not in the scope of this function?
                // use this-> to access I3Converter::currentFrame_ TODO: test that this works correctly!
                unsigned rowsWritten = itemConverter_->Convert(*it, rows, this->currentFrame_);

                // for all rows the itemConverter has changed the vector_index
                // field must be set to current index
                for (unsigned int j_row = 0; j_row < rowsWritten; ++j_row) {
                    rows->SetCurrentRow(i_row+j_row);
                    rows->Set<unsigned int>("vector_index", v_index);
                }

                i_row += rowsWritten;
                v_index++;
            }

            // check that nobody changed the row size
            assert(nrows == rows->GetNumberOfRows());

            // loop over vector
            return i_row;
        }

    std::string  converterName_;
    I3ConverterPtr itemConverter_;
};
