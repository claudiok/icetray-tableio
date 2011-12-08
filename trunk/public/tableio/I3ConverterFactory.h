/*
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

/**
 * \file I3ConverterFactory.h
 * \brief Contains classes, functions and macros to handle the converter registry.
 *
 * The converter registry is used by tableio to keep track of all available
 * converters. All the user needs to do is register his converter with the help of
 * the I3_CONVERTER() macro.
 */

#include "icetray/I3Factory.h"
#include "tableio/I3Converter.h"

/******************************************************************************/
//
// basically copy and paste from icetray/I3Factory.h . The original Registrator
// assumed a factory function that takes the context. 
// extended I3Registrator to take a 4th template argument. this could go back into icetray

/**
 * \brief Registers converters with I3ConverterFactory
 *
 * Used by the I3_CONVERTER macro.
 */
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

/**
 * \brief Creates an instance of a converter
 *
 * Used by the I3_CONVERTER macro
 */
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

/// The converter factory is used by tableio to create converters.
typedef I3Factory<I3Converter, I3Converter_ffn_t> I3ConverterFactory;

/******************************************************************************/

/**
 * \brief Use this macro to register a converter with tableio
 *
 * \param CONVERTER The converter class
 * \param REGISTRY_STRING A name for the converter. Typically the name of the 
 *                        class to be converted.
 *
 * Example:
 * <pre>
 *   I3_CONVERTER(I3LineFitParamsConverter, I3LineFitParams)
 * </pre>
 */
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

/// Constructs an instance of a converter for a given name.
I3ConverterPtr BuildConverter(std::string name);

/******************************************************************************/

#endif

