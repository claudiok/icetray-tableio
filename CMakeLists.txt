#
# $Id: CMakeLists.txt 58879 2009-10-15 12:40:18Z panknin $
#

i3_project(hdf-writer 
	PYTHON_DIR python
	PYTHON_DEST icecube/hdf
	DOCS_DIR resources/docs
)

# support HDF5 installations outside of ports
# this should really be done with an hdf5.cmake in I3_SITE_CMAKE_DIR
INCLUDE(standalone_hdf5.cmake)

IF(HDF5_FOUND)

INCLUDE(AddConverter.cmake)
# core dependencies of the writer
SET(${PROJECT_NAME}_PROJECTS icetray)
# core source files of the writer
SET(${PROJECT_NAME}_SOURCEFILES private/hdf-writer/*.cxx private/hdf-writer/tools/*.cxx private/hdf-writer/converter/*.cxx private/hdf-writer/hdf/*.cxx )

# ====================================================================================
# = Add your converters here. They will only be built if the dependencies are found. =
# = Chances are, if you don't have the project installed, you don't care whether or  =
# = not you can write its frame objects to HDF.                                      =
# ====================================================================================

HDFWRITER_ADD_CONVERTER(I3EventHeader DOMInfo EventInfo I3Particle
                        I3MCTree I3MCWeightDict I3Int I3Double I3Bool I3Waveform
                        I3CorsikaWeightMap I3Pulses I3MCHits I3Geometry I3Trigger
                        I3FlasherInfo TWREventInfo DEPENDS_ON dataclasses)
HDFWRITER_ADD_CONVERTER(I3FilterMask DEPENDS_ON jebclasses)
HDFWRITER_ADD_CONVERTER(I3RecoInfo I3CascadeCutValues DEPENDS_ON dataclasses phys-services)
HDFWRITER_ADD_CONVERTER(I3LineFitParams DEPENDS_ON linefit)
HDFWRITER_ADD_CONVERTER(I3TensorOfInertiaFitParams DEPENDS_ON tensor-of-inertia)
HDFWRITER_ADD_CONVERTER(I3DipoleFitParams DEPENDS_ON dipolefit)
HDFWRITER_ADD_CONVERTER(I3CFirstFitParams DEPENDS_ON cfirst)
HDFWRITER_ADD_CONVERTER(I3CFlashFitParams DEPENDS_ON cflash)
HDFWRITER_ADD_CONVERTER(I3LogLikelihoodFitParams DEPENDS_ON gulliver)
HDFWRITER_ADD_CONVERTER(I3FillRatioInfo DEPENDS_ON fill-ratio)
HDFWRITER_ADD_CONVERTER(I3ParaboloidFitParams DEPENDS_ON paraboloid)
HDFWRITER_ADD_CONVERTER(I3CscdLlhFitParams DEPENDS_ON cscd-llh)
HDFWRITER_ADD_CONVERTER(I3CredoDiagStats DEPENDS_ON credo)
HDFWRITER_ADD_CONVERTER(I3CredoDiagCache DEPENDS_ON credo)
HDFWRITER_ADD_CONVERTER(I3CutValues DEPENDS_ON phys-services)

# uniquify the list of projects to link against 
# (only really necessary if some converters depend on multiple projects)
UNIQUE(${PROJECT_NAME}_PROJECTS "${${PROJECT_NAME}_PROJECTS}")

#######################################################################
#
# Usual i3-project definition
#
#######################################################################


if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})
  # add preprocessor flags to prevent compilation in ModuleExport.cxx
  ADD_DEFINITIONS(${${PROJECT_NAME}_DEFINES})

  i3_add_library(hdf-writer
	${${PROJECT_NAME}_SOURCEFILES}

    USE_TOOLS log4cplus boost hdf5
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS ${${PROJECT_NAME}_PROJECTS})

  add_subdirectory(private/pybindings)

  # add the path to the place where the hdf5 libs exist
  #TARGET_LINK_LIBRARIES(hdf-writer ${HDF5_LIBRARIES})
  i3_test_executable(test 
    private/test/*.cxx
    USE_PROJECTS dataclasses icetray phys-services hdf-writer)


ELSE(HDF5_FOUND)

  MESSAGE(STATUS "*** hdf5 library not found. 
   *** Ask your admin to install the hdf5 package from IceCube ports.")
  MESSAGE(STATUS "Skipping build of ${PROJECT_NAME}")

ENDIF(HDF5_FOUND)

