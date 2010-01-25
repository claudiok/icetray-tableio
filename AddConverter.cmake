# 
# Macros to support conditional compilation of HDFWriter converters
# 
# copyright  (C) 2009
# the icecube collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <jakob.van.santen@desy.de>   Last changed by: $LastChangedBy$
# 

macro(HDFWRITER_ADD_CONVERTER)
	parse_arguments(HDFWRITER_ADD_CONVERTER
	"DEPENDS_ON"
	""
	${ARGN}
	)
	# check if dependencies exist in the source tree
	SET(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND TRUE)
	FOREACH(USED_PROJECT ${HDFWRITER_ADD_CONVERTER_DEPENDS_ON})
		if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT})
			SET(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND FALSE)
			#SET(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND TRUE)
		else()
			#SET(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND FALSE)
		endif(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${USED_PROJECT})
	ENDFOREACH(USED_PROJECT ${HDFWRITER_ADD_CONVERTER_DEPENDS_ON})
	# add converters to the list of files to be compiled only if their dependencies were found 
	if(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND)
		MESSAGE(STATUS "     + adding converters for ${HDFWRITER_ADD_CONVERTER_DEFAULT_ARGS}")
		LIST(APPEND ${PROJECT_NAME}_PROJECTS ${HDFWRITER_ADD_CONVERTER_DEPENDS_ON})
		FOREACH(HDFWRITER_ADD_CONVERTER_CONVERTER ${HDFWRITER_ADD_CONVERTER_DEFAULT_ARGS})
			# compile the implementation file if it exists
			IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/private/hdf-writer/converter/${HDFWRITER_ADD_CONVERTER_CONVERTER}ToTable.cxx)
				LIST(APPEND ${PROJECT_NAME}_SOURCEFILES private/hdf-writer/converter/${HDFWRITER_ADD_CONVERTER_CONVERTER}ToTable.cxx)
			ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/private/hdf-writer/converter/${HDFWRITER_ADD_CONVERTER_CONVERTER}ToTable.cxx)
			# add preprocessor flags for use in ModuleExport.cxx
			LIST(APPEND ${PROJECT_NAME}_DEFINES -DUSE_${HDFWRITER_ADD_CONVERTER_CONVERTER})
		ENDFOREACH(HDFWRITER_ADD_CONVERTER_CONVERTER ${HDFWRITER_ADD_CONVERTER_DEFAULT_ARGS})
	else()
		MESSAGE(STATUS "     - omitting converters for ${HDFWRITER_ADD_CONVERTER_DEFAULT_ARGS} (project ${HDFWRITER_ADD_CONVERTER_DEPENDS_ON} could not be found)")
	endif(HDFWRITER_ADD_CONVERTER_PROJECTS_FOUND)

endmacro(HDFWRITER_ADD_CONVERTER)

# lifted from http://aliceinfo.cern.ch/static/aliroot-new/cmake/modules/CbmMacros.cmake
MACRO(UNIQUE var_name list)

  #######################################################################
  # Make the given list have only one instance of each unique element and
  # store it in var_name.
  #######################################################################

  SET(unique_tmp "")
  FOREACH(l ${list})
    STRING(REGEX REPLACE "[+]" "\\\\+" l1 ${l})
    IF(NOT "${unique_tmp}" MATCHES "(^|;)${l1}(;|$)")
      SET(unique_tmp ${unique_tmp} ${l})
    ENDIF(NOT "${unique_tmp}" MATCHES "(^|;)${l1}(;|$)")
  ENDFOREACH(l)
  SET(${var_name} ${unique_tmp})
ENDMACRO(UNIQUE)
