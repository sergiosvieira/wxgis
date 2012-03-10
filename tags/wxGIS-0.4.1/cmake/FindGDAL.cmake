# - Try to find the GDAL encryption library
# Once done this will define
#
#  GDAL_ROOT_DIR - Set this variable to the root installation of GDAL
#
# Read-Only variables:
#  GDAL_FOUND - system has the GDAL library
#  GDAL_INCLUDE_DIR - the GDAL include directory
#  GDAL_LIBRARIES - The libraries needed to use GDAL
#  GDAL_VERSION - This is set to $major.$minor.$revision (eg. 0.9.8)

#=============================================================================
# Copyright 2012 Dmitry Baryshnikov <polimax at mail dot ru>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if (UNIX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_GDAL gdal)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

SET(_GDAL_ROOT_HINTS
  $ENV{GDAL}
  $ENV{GDAL_ROOT}
  $ENV{LIB}
  $ENV{LIB_HOME}/gdal
  ${GDAL_ROOT_DIR}
  )
SET(_GDAL_ROOT_PATHS
  $ENV{GDAL}
  $ENV{GDAL_ROOT}
  $ENV{LIB}
  $ENV{LIB_HOME}/gdal
  )
SET(_GDAL_ROOT_HINTS_AND_PATHS
  HINTS ${_GDAL_ROOT_HINTS}
  PATHS ${_GDAL_ROOT_PATHS}
  )

FIND_PATH(GDAL_INCLUDE_DIR
  NAMES
    gcore/gdal.h
  HINTS
    ${_GDAL_INCLUDEDIR}
  ${_GDAL_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    include
)

IF(WIN32 AND NOT CYGWIN)
  # MINGW should go here too
  IF(MSVC)
    # Implementation details:
    # We are using the libraries located in the VC subdir instead of the parent directory eventhough :
    FIND_LIBRARY(GDAL_DEBUG
      NAMES
        gdal18d
        gdal19d
        gdal19d
        gdal20d
        gdal21d
        gdal22d
        gdal23d
        gdal24d
        gdal25d
        gdal26d
        gdal27d
        gdal28d
        gdal29d
        gdal30d
      ${_GDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    FIND_LIBRARY(GDAL_RELEASE
      NAMES
        gdal18
        gdal19
        gdal19
        gdal20
        gdal21
        gdal22
        gdal23
        gdal24
        gdal25
        gdal26
        gdal27
        gdal28
        gdal29
        gdal30
      ${_GDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
        if(NOT GDAL_DEBUG)
            set(GDAL_DEBUG ${GDAL_RELEASE})
        endif(NOT GDAL_DEBUG)     
      set( GDAL_LIBRARIES
        optimized ${GDAL_RELEASE} debug ${GDAL_DEBUG}
        )
    else()
      set( GDAL_LIBRARIES ${GDAL_RELEASE})
    endif()
    MARK_AS_ADVANCED(GDAL_DEBUG GDAL_RELEASE)
  ELSEIF(MINGW)
    # same player, for MingW
    FIND_LIBRARY(GDAL
      NAMES        
        gdal18
        gdal19
        gdal19
        gdal20
        gdal21
        gdal22
        gdal23
        gdal24
        gdal25
        gdal26
        gdal27
        gdal28
        gdal29
        gdal30
      ${_GDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "lib/MinGW"
    )

    MARK_AS_ADVANCED(GDAL)
    set( GDAL_LIBRARIES ${GDAL})
  ELSE(MSVC)
    # Not sure what to pick for -say- intel, let's use the toplevel ones and hope someone report issues:
    FIND_LIBRARY(GDAL
      NAMES
        gdal18
        gdal19
        gdal19
        gdal20
        gdal21
        gdal22
        gdal23
        gdal24
        gdal25
        gdal26
        gdal27
        gdal28
        gdal29
        gdal30
      HINTS
        ${_GDAL_LIBDIR}
      ${_GDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        lib
    ) 

    MARK_AS_ADVANCED(GDAL)
    set( GDAL_LIBRARIES ${GDAL} )
  ENDIF(MSVC)
ELSE(WIN32 AND NOT CYGWIN)

  FIND_LIBRARY(GDAL_LIBRARY
    NAMES
        gdal18
        gdal19
        gdal19
        gdal20
        gdal21
        gdal22
        gdal23
        gdal24
        gdal25
        gdal26
        gdal27
        gdal28
        gdal29
        gdal30
    HINTS
      ${_GDAL_LIBDIR}
    ${_GDAL_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      lib
  ) 

  MARK_AS_ADVANCED(GDAL_LIBRARY)

  # compat defines
  SET(GDAL_LIBRARIES ${GDAL_LIBRARY})

ENDIF(WIN32 AND NOT CYGWIN)

 if (GDAL_INCLUDE_DIR) 
    file(READ "${GDAL_INCLUDE_DIR}/gcore/gdal_version.h" _gdal_VERSION_H_CONTENTS)
    string(REGEX REPLACE ".*#  define[ \t]+GDAL_RELEASE_NAME[ \t]+\"([0-9A-Za-z.]+)\".*"
    "\\1" GDAL_VERSION ${_gdal_VERSION_H_CONTENTS})
    set(GDAL_VERSION ${GDAL_VERSION} CACHE INTERNAL "The version number for gdal libraries")
 endif (GDAL_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)

if (GDAL_VERSION)
  find_package_handle_standard_args(GDAL
    REQUIRED_VARS
      GDAL_LIBRARIES
      GDAL_INCLUDE_DIR
    VERSION_VAR
      GDAL_VERSION
    FAIL_MESSAGE
      "Could NOT find GDAL, try to set the path to GDAL root folder in the system variable GDAL_ROOT_DIR"
  )
else (GDAL_VERSION)
  find_package_handle_standard_args(GDAL "Could NOT find GDAL, try to set the path to GDAL root folder in the system variable GDAL"
    GDAL_LIBRARIES
    GDAL_INCLUDE_DIR
  )
endif (GDAL_VERSION)

MARK_AS_ADVANCED(GDAL_INCLUDE_DIR GDAL_LIBRARIES)