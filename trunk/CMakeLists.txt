# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Bishop
# *
# *    This program is free software: you can redistribute it and/or modify
# *    it under the terms of the GNU General Public License as published by
# *    the Free Software Foundation, either version 3 of the License, or
# *    (at your option) any later version.
# *
# *    This program is distributed in the hope that it will be useful,
# *    but WITHOUT ANY WARRANTY; without even the implied warranty of
# *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *    GNU General Public License for more details.
# *
# *    You should have received a copy of the GNU General Public License
# *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************
cmake_minimum_required (VERSION 2.8)
project (wxgis)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
set(CMAKE_COLOR_MAKEFILE ON)

# parse the version number from gdal_version.h and include in wxGIS_MAJOR_VERSION and wxGIS_MINOR_VERSION
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/wxgis/version.h WXGIS_VERSION_H_CONTENTS)
string(REGEX MATCH "wxGIS_MAJOR_VERSION[ \t]+([0-9]+)"
  wxGIS_MAJOR_VERSION ${WXGIS_VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
  wxGIS_MAJOR_VERSION ${wxGIS_MAJOR_VERSION})
string(REGEX MATCH "wxGIS_MINOR_VERSION[ \t]+([0-9]+)"
  wxGIS_MINOR_VERSION ${WXGIS_VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
  wxGIS_MINOR_VERSION ${wxGIS_MINOR_VERSION})
string(REGEX MATCH "wxGIS_RELEASE_NUMBER[ \t]+([0-9]+)"
  wxGIS_RELEASE_NUMBER ${WXGIS_VERSION_H_CONTENTS})  
string (REGEX MATCH "([0-9]+)"
  wxGIS_RELEASE_NUMBER ${wxGIS_RELEASE_NUMBER})

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
    
# Setup package meta-data
set(WXGIS_VERSION ${wxGIS_MAJOR_VERSION}.${wxGIS_MINOR_VERSION}.${wxGIS_RELEASE_NUMBER})
message(STATUS "wxgis version=[${WXGIS_VERSION}]")
message(STATUS "c++ compiler ... " ${CMAKE_CXX_COMPILER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(WXGIS_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WXGIS_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

#configure_file(${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/defs.h.cmake ${WXGIS_CURRENT_BINARY_DIR}/wxgisdefs.h @ONLY)
configure_file(${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/defs.h.cmake ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/defs.h @ONLY)

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# For windows, do not allow the compiler to use default target (Vista).
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

add_definitions(-DSTRICT)

#find needed packages
#wxgisgdal deprecated
option(wxGIS_USE_WXGISGDAL "Set OFF to build using classical GDAL, else - adapted version wxGISGDAL" ON)
if(wxGIS_USE_WXGISGDAL)
    find_package(WXGISGDAL REQUIRED)
else(wxGIS_USE_WXGISGDAL)
    find_package(GDAL REQUIRED)
endif(wxGIS_USE_WXGISGDAL)

if(WXGISGDAL_FOUND)
    include_directories(${WXGISGDAL_INCLUDE_DIR})
    include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/port)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/gcore)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/alg)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/frmts/vrt)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    include_directories(${GDAL_INCLUDE_DIR}/ogr)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
    include_directories(${GDAL_INCLUDE_DIR}/port)
    include_directories(${GDAL_INCLUDE_DIR}/gcore)
    include_directories(${GDAL_INCLUDE_DIR}/alg)
    include_directories(${GDAL_INCLUDE_DIR}/frmts/vrt)
    include_directories(${GDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
endif(GDAL_FOUND)

option(wxGIS_PORTABLE "Set OFF to build non portable version" ON)
if(wxGIS_PORTABLE)
    add_definitions(-DWXGISPORTABLE)
endif(wxGIS_PORTABLE)

if(MSVC)
    #string(REPLACE "/W4" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    #string(REPLACE "/W4" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # string(REPLACE "/Gm" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    #set(CMAKE_CXX_FLAGS_DEBUG "/Gm /ZI /W3 /Od")
    # add_definitions(-DDISABLE_SOME_FLOATING_POINT)
    # set_target_properties( ${the_target} PROPERTIES COMPILE_FLAGS "/Gm" )
    if(CMAKE_CL_64)
        set_target_properties(${the_target} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
        add_definitions(-D_WIN64)
    endif()
endif()

if(UNIX)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")#-Wextra -Wall -W -pthread -O2 -fno-strict-aliasing -pthrea
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-strict-aliasing")
     endif()
    # Select flags.
#    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g")
#    set(CMAKE_CXX_FLAGS_RELEASE        "-O")
#    set(CMAKE_CXX_FLAGS_DEBUG          "-Wall -g -O")
#    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O0 -g")
#    set(CMAKE_C_FLAGS_RELEASE          "-O")
#    set(CMAKE_C_FLAGS_DEBUG            "-Wall -g -O2")
endif()

#core
set(WXGISCORE_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/core/)
#networking
set(WXGISNET_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/net/)
#task manager 
option(wxGIS_HAVE_TASKMANAGER "Set ON to build gis task manager" ON)
if(wxGIS_HAVE_TASKMANAGER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tskmngr_app/)
endif(wxGIS_HAVE_TASKMANAGER)

option(wxGIS_HAVE_CATALOG "Set ON to build catalog and datasource" ON)
if(wxGIS_HAVE_CATALOG)
    #datasource
    set(WXGISDATASOURCE_LIB_NAME "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/datasource/)
    #catalog
    set(WXGISCATALOG_LIB_NAME "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/catalog/)
    
    option(wxGIS_HAVE_GEOPROCESSING "Set ON to build geoprocessing" ON)
    if(wxGIS_HAVE_GEOPROCESSING)
        #geoprocessing
        set(WXGISGEOPROCESSING_LIB_NAME "")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocessing/)
    endif(wxGIS_HAVE_GEOPROCESSING)
endif(wxGIS_HAVE_CATALOG)

option(wxGIS_HAVE_DESKTOP "Set ON to build desktop version" ON)
if(wxGIS_HAVE_DESKTOP)
#display
set(WXGISDISPLAY_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/display/)
#framework
set(WXGISFRAMEWORK_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/framework/)
#carto
set(WXGISCARTO_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/carto/)
#cartoui
set(WXGISCARTOUI_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cartoui/)

#catalogui
set(WXGISCATALOGUI_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/catalogui/)

if(wxGIS_HAVE_GEOPROCESSING)
    #geoprocessingui
    set(WXGISGEOPROCESSINGUI_LIB_NAME "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocessingui/)
endif(wxGIS_HAVE_GEOPROCESSING)    

#cat_app
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cat_app/)
#geoprocess
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocess/)
# === new ads ===

option(wxGIS_HAVE_REMOTE "Set ON to build remote server support" OFF)
if(wxGIS_HAVE_REMOTE)
#remoteserver
set(WXGISRS_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/remoteserver/)
#remoteserverui
set(WXGISRSU_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/remoteserverui/)
endif(wxGIS_HAVE_REMOTE)
endif(wxGIS_HAVE_DESKTOP)

option(wxGIS_HAVE_SERVER "Set ON to build server version" ON)
if(wxGIS_CATALOG AND wxGIS_HAVE_SERVER)
#srv_framework
set(WXGISSRVFRW_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/srv_framework/)
#srv_networking deprecated
#set(WXGISSRVNET_LIB_NAME "")
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/srv_net/)
#srv_app
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/srv_app/)
endif(wxGIS_CATALOG AND wxGIS_HAVE_SERVER)

if(WIN32)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/locale CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/locale CONFIGURATIONS Release FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
    install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Debug/log)"  CONFIGURATIONS Debug)
    install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Release/log)"  CONFIGURATIONS Release)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/sys CONFIGURATIONS Debug)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/sys CONFIGURATIONS Release)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config CONFIGURATIONS Debug)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/config CONFIGURATIONS Release)
else(WIN32)#UNIX
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_loc/ DESTINATION share/locale/wxgis/ FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt" )
    install(CODE "FILE(MAKE_DIRECTORY /var/log/wxgis)")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/ DESTINATION share/wxgis)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_config/ DESTINATION /etc/wxgis)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/wxgis DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    install(DIRECTORY ${WXGIS_CURRENT_BINARY_DIR} DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
endif(WIN32)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

