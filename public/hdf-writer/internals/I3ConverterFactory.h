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

#ifndef	I3CONVERTERFACTORY_H_INCLUDED
#define I3CONVERTERFACTORY_H_INCLUDED

#include "icetray/I3Factory.h"
#include "hdf-writer/internals/I3Converter.h"

/******************************************************************************/
//
// basically copy and paste from icetray/I3Factory.h . The original Registrator
// assumed a factory function that takes the context. 
// extended I3Registrator to take a 4th template argument. this could go back into icetray

template <class FactoryProductType, 
          class ActualDerivedType, 
	      template <class, class> class Creator,
          class FactoryFunctionType>
struct I3ConverterRegistrator : boost::noncopyable
{
  I3ConverterRegistrator& key_register(const char* productname,
			                           const char* projectname) BOOST_USED
  {
    log_trace("key_register %s %s", productname, projectname);

    typedef I3Factory<FactoryProductType,FactoryFunctionType> factory_t;
    
    I3::Singleton<factory_t>::get_mutable_instance()
      .Register(productname, 
		projectname, 
		Creator<FactoryProductType, ActualDerivedType>::Create);
    I3DSORegistry::register_dso(projectname);
    return *this;
  }

  // this should technically be private (see friend declaration below)
  // but some gccs get confused.
  I3ConverterRegistrator() {};
private:

  friend class I3::Singleton<I3ConverterRegistrator>;
};

/******************************************************************************/


template <class FactoryProductType, class ConverterType>
struct ConverterCreator {
  static shared_ptr<FactoryProductType> Create () {
    shared_ptr<FactoryProductType> converter(new ConverterType());
    if (!converter) 
      log_fatal("failed to create");
    return converter;
  }
};

/******************************************************************************/

typedef boost::function<I3ConverterPtr()> I3Converter_ffn_t;
typedef I3Factory<I3Converter, I3Converter_ffn_t> I3ConverterFactory;

/******************************************************************************/

#define I3_CONVERTER(CONVERTER, REGISTRY_STRING)				\
    namespace {								\
        ::I3ConverterRegistrator<I3Converter, CONVERTER, ConverterCreator,\
                                 I3Converter_ffn_t> const&			\
        BOOST_PP_CAT(I3Registrator_, __LINE__)				\
        = I3::Singleton<I3ConverterRegistrator<I3Converter, CONVERTER,\
                                               ConverterCreator, I3Converter_ffn_t> > \
        ::get_mutable_instance().key_register(BOOST_PP_STRINGIZE(REGISTRY_STRING),	\
                BOOST_PP_STRINGIZE(PROJECT)); \
    }									

/******************************************************************************/
I3ConverterPtr BuildConverter(std::string name, StringPairVector params);

I3ConverterPtr BuildConverter(std::string name);

/******************************************************************************/

#endif

