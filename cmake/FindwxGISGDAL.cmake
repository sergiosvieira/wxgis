# - Try to find the WXGISGDAL library
# Once done this will define
#
#  WXGISGDAL_ROOT_DIR - Set this variable to the root installation of WXGISGDAL
#
# Read-Only variables:
#  WXGISGDAL_FOUND - system has the WXGISGDAL library
#  WXGISGDAL_INCLUDE_DIR - the WXGISGDAL include directory
#  WXGISGDAL_LIBRARIES - The libraries needed to use WXGISGDAL
#  WXGISGDAL_VERSION - This is set to $major.$minor.$revision (eg. 0.9.8)

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
    pkg_check_modules(_WXGISGDAL wxgisgdal)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

SET(_WXGISGDAL_ROOT_HINTS
  $ENV{WXGISGDAL}
  ${WXGISGDAL_ROOT_DIR}
  )
SET(_WXGISGDAL_ROOT_PATHS
  $ENV{WXGISGDAL}
  )
SET(_WXGISGDAL_ROOT_HINTS_AND_PATHS
  HINTS ${_WXGISGDAL_ROOT_HINTS}
  PATHS ${_WXGISGDAL_ROOT_PATHS}
  )

FIND_PATH(WXGISGDAL_INCLUDE_DIR
  NAMES
    gcore/gdal.h
  HINTS
    ${_WXGISGDAL_INCLUDEDIR}
  ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    include
)

IF(WIN32 AND NOT CYGWIN)
  # MINGW should go here too
  IF(MSVC)
    # Implementation details:
    # We are using the libraries located in the VC subdir instead of the parent directory eventhough :
    FIND_LIBRARY(WXGISGDAL_DEBUG
      NAMES
        wxgisgdal18d
        wxgisgdal19d
        wxgisgdal19d
        wxgisgdal20d
        wxgisgdal21d
        wxgisgdal22d
        wxgisgdal23d
        wxgisgdal24d
        wxgisgdal25d
        wxgisgdal26d
        wxgisgdal27d
        wxgisgdal28d
        wxgisgdal29d
        wxgisgdal30d
      ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    FIND_LIBRARY(WXGISGDAL_RELEASE
      NAMES
        wxgisgdal18
        wxgisgdal19
        wxgisgdal19
        wxgisgdal20
        wxgisgdal21
        wxgisgdal22
        wxgisgdal23
        wxgisgdal24
        wxgisgdal25
        wxgisgdal26
        wxgisgdal27
        wxgisgdal28
        wxgisgdal29
        wxgisgdal30
      ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
        if(NOT WXGISGDAL_DEBUG)
            set(WXGISGDAL_DEBUG ${WXGISGDAL_RELEASE})
        endif(NOT WXGISGDAL_DEBUG)    
      set( WXGISGDAL_LIBRARIES
        optimized ${WXGISGDAL_RELEASE} debug ${WXGISGDAL_DEBUG}
        )
    else()
      set( WXGISGDAL_LIBRARIES ${WXGISGDAL_RELEASE})
    endif()
    MARK_AS_ADVANCED(WXGISGDAL_DEBUG WXGISGDAL_RELEASE)
  ELSEIF(MINGW)
    # same player, for MingW
    FIND_LIBRARY(WXGISGDAL
      NAMES        
        wxgisgdal18
        wxgisgdal19
        wxgisgdal19
        wxgisgdal20
        wxgisgdal21
        wxgisgdal22
        wxgisgdal23
        wxgisgdal24
        wxgisgdal25
        wxgisgdal26
        wxgisgdal27
        wxgisgdal28
        wxgisgdal29
        wxgisgdal30
      ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "lib/MinGW"
    )

    MARK_AS_ADVANCED(WXGISGDAL)
    set( WXGISGDAL_LIBRARIES ${WXGISGDAL})
  ELSE(MSVC)
    # Not sure what to pick for -say- intel, let's use the toplevel ones and hope someone report issues:
    FIND_LIBRARY(WXGISGDAL
      NAMES
        wxgisgdal18
        wxgisgdal19
        wxgisgdal19
        wxgisgdal20
        wxgisgdal21
        wxgisgdal22
        wxgisgdal23
        wxgisgdal24
        wxgisgdal25
        wxgisgdal26
        wxgisgdal27
        wxgisgdal28
        wxgisgdal29
        wxgisgdal30
      HINTS
        ${_WXGISGDAL_LIBDIR}
      ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        lib
    ) 

    MARK_AS_ADVANCED(WXGISGDAL)
    set( WXGISGDAL_LIBRARIES ${WXGISGDAL} )
  ENDIF(MSVC)
ELSE(WIN32 AND NOT CYGWIN)

  FIND_LIBRARY(WXGISGDAL_LIBRARY
    NAMES
        wxgisgdal18
        wxgisgdal19
        wxgisgdal19
        wxgisgdal20
        wxgisgdal21
        wxgisgdal22
        wxgisgdal23
        wxgisgdal24
        wxgisgdal25
        wxgisgdal26
        wxgisgdal27
        wxgisgdal28
        wxgisgdal29
        wxgisgdal30
    HINTS
      ${_WXGISGDAL_LIBDIR}
    ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      lib
  ) 

  MARK_AS_ADVANCED(WXGISGDAL_LIBRARY)

  # compat defines
  SET(WXGISGDAL_LIBRARIES ${WXGISGDAL_LIBRARY})

ENDIF(WIN32 AND NOT CYGWIN)

 if (WXGISGDAL_INCLUDE_DIR) 
    file(READ "${WXGISGDAL_INCLUDE_DIR}/gcore/gdal_version.h" _wxgisgdal_VERSION_H_CONTENTS)
    string(REGEX REPLACE ".*#  define[ \t]+GDAL_RELEASE_NAME[ \t]+\"([0-9A-Za-z.]+)\".*"
    "\\1" WXGISGDAL_VERSION ${_wxgisgdal_VERSION_H_CONTENTS})
    set(WXGISGDAL_VERSION ${WXGISGDAL_VERSION} CACHE INTERNAL "The version number for wxgisgdal libraries")
 endif (WXGISGDAL_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)

if (WXGISGDAL_VERSION)
  find_package_handle_standard_args(WXGISGDAL
    REQUIRED_VARS
      WXGISGDAL_LIBRARIES
      WXGISGDAL_INCLUDE_DIR
    VERSION_VAR
      WXGISGDAL_VERSION
    FAIL_MESSAGE
      "Could NOT find WXGISGDAL, try to set the path to WXGISGDAL root folder in the system variable WXGISGDAL_ROOT_DIR"
  )
else (WXGISGDAL_VERSION)
  find_package_handle_standard_args(WXGISGDAL "Could NOT find WXGISGDAL, try to set the path to WXGISGDAL root folder in the system variable WXGISGDAL"
    WXGISGDAL_LIBRARIES
    WXGISGDAL_INCLUDE_DIR
  )
endif (WXGISGDAL_VERSION)

MARK_AS_ADVANCED(WXGISGDAL_INCLUDE_DIR WXGISGDAL_LIBRARIES)
