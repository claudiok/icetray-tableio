.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$

Writing a converter for an I3Map or an I3Vector
***************************************************

.. highlight:: c++

In order to simplify the creation of converters for objects of type I3...Series
or I3...SeriesMap (i.e. I3Map<OMKey, std::vector<...> >), two converter
templates have been implemented in C++: I3VectorConverter and
I3MapOMKeyVectorConverter. The template parameter is a simple struct to convert
the stored data. This struct needs to fulfill the following requirements:

* define a type called value_type, which is the type converted by
  this struct;
* has two functions
  ::
    void AddFields(I3TableRowDescriptionPtr);
    void AddFields(I3TableRowDescriptionPtr, const value_type&);
 
  that add the required fields to the given I3TableRowDescription - the first
  one can of course be replaced by adding a default value to the second
  parameter of the second version; and
* has a function
  ::
    void FillSingleRow(const value_type&, I3TableRowPtr);

  that takes a single object of value_type and fills the given table row.

The same struct can be used both for the I3VectorConverter and the
I3MapOMKeyVectorConverter.

.. note::
    I3MapConverter has a parameter bookGeometry in its constructor that defaults
    to false and allows users to bookGeometry together with the data in the map.
    To make this option available in python, use the I3_MAP_CONVERTER_EXPORT or
    I3_MAP_CONVERTER_EXPORT_DEFAULT macros instead of I3CONVERTER_EXPORT or
    I3CONVERTER_EXPORT_DEFAULT.



Example: I3RecoPulseSeriesMapConverter
_________________________________________

.. highlight:: c++

The following example shows the conversion struct for the I3RecoPulseSeriesMap
converter. Mind the required typedef value_type.
::

    struct convert_I3RecoPulse {

      typedef I3RecoPulse value_type;

      void AddFields(I3TableRowDescriptionPtr desc, const I3RecoPulse& = I3RecoPulse())
      {
        desc->AddField<double>("time", "ns", "Leading-edge time of the pulse");
    	desc->AddField<double>("width", "ns", "Duration of the pulse");
    	desc->AddField<double>("charge", "PE", "Integrated pulse charge");
    	desc->AddField<int32_t>("id", "generic", "hit id");
      }

      void I3RecoPulse::FillSingleRow(const value_type& pulse, I3TableRowPtr row)
      {
        row->Set<double>("time", pulse.GetTime());
    	row->Set<double>("width", pulse.GetWidth());
    	row->Set<double>("charge", pulse.GetCharge());
    	row->Set<int32_t>("id", pulse.GetID());
      }
    
    };

    I3_CONVERTER(I3MapOMKeyVectorConverter< convert_I3RecoPulse >, I3RecoPulseSeriesMap);
