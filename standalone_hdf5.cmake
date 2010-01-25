#
# $Id$
#

################################################################################
# This macro searches for the hdf5 library and the include directory
# Once done this will define
# 
# HDF5_FOUND
# HDF5_INCLUDE_DIR
# HDF5_LIBRARIES
################################################################################

IF(NOT HDF5_FOUND)
MESSAGE(STATUS "
        hdf5 not found in I3_PORTS. Searching for hdf5 lib and headers.
        You can also hard-code the proper paths for your system in a
        file called hdf5.cmake in your I3_SITE_CMAKE_DIR.
")

# standard hdf5 lib
FIND_LIBRARY(_hdf5lib
             NAMES hdf5 hdf5lib
 	     PATHS ENV LD_LIBRARY_PATH)
REPORT_FIND(hdf5 "hdf5 lib" ${_hdf5lib})

# hdf5 high level lib
FIND_LIBRARY(_hdf5hllib
             NAMES hdf5_hl hdf5lib_hl
             PATHS ENV LD_LIBRARY_PATH)
REPORT_FIND(hdf5 "hdf5_hl lib" ${_hdf5hllib})

# get include dir
IF(_hdf5lib AND _hdf5hllib)
    # get folder containing lib and guess include path
    STRING(REGEX REPLACE "(.*)/.+\\..+" "\\1" _hdf5_lib_dir ${_hdf5lib})
    SET(_hdf5_include_path "${_hdf5_lib_dir}/../include")
    FIND_PATH(HDF5_INCLUDE_DIR 
              NAMES hdf5.h
	      PATHS ${_hdf5_include_path} ENV INCLUDE_DIR
	      PATH_SUFFIXES hdf5)
    REPORT_FIND(hdf5 "hdf5.h" ${HDF5_INCLUDE_DIR})
				
    # combine found hdf5 libs
    SET(HDF5_LIBRARIES ${_hdf5lib} ${_hdf5hllib} CACHE PATH "Libraries for HDF5 - standalone" FORCE)

    IF(HDF5_LIBRARIES AND HDF5_INCLUDE_DIR)
      SET(HDF5_FOUND TRUE CACHE BOOL "Standalone HDF5 found successfully" FORCE)
      SET(HDF5_CONFIG_ERROR CACHE FALSE BOOL FORCE)
      MESSAGE(STATUS "    Found hdf5")
    ELSE(HDF5_LIBRARIES AND HDF5_INCLUDE_DIR)
      SET(HDF5_FOUND FALSE CACHE)
    ENDIF(HDF5_LIBRARIES AND HDF5_INCLUDE_DIR)

ENDIF(_hdf5lib AND _hdf5hllib)
ENDIF(NOT HDF5_FOUND)

IF(NOT HDF5_FOUND)

  MESSAGE(STATUS "*** hdf5 library not found. 
  *** Ask your admin to install hdf5 development package
  *** or download from http://hdf.ncsa.uiuc.edu/HDF5/ 
  *** and install manually. If not installed in a standard location,
  *** try setting the LD_LIBRARY_PATH environment variable to the 
  *** folder where the hdf5 library file is located. If only the
  *** header file is not found, set the INCLUDE_DIR environment variable
  *** to the folder where the hdf5.h file is located.")
  MESSAGE(STATUS "Skipping build of ${PROJECT_NAME}.")

ENDIF(NOT HDF5_FOUND)

