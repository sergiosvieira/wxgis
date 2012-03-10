cmake_minimum_required (VERSION 2.6)
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
    
# Setup package meta-data
set(WXGIS_VERSION ${wxGIS_MAJOR_VERSION}.${wxGIS_MINOR_VERSION}.${wxGIS_RELEASE_NUMBER})
message(STATUS "wxgis version=[${WXGIS_VERSION}]")
message(STATUS "c++ compiler ... " ${CMAKE_CXX_COMPILER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(WXGIS_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WXGIS_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# For windows, do not allow the compiler to use default target (Vista).
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

add_definitions(-DSTRICT)

#find needed packages
option(wxGIS_USE_WXGISGDAL "Set OFF to build using classical GDAL, else - adapted version wxGISGDAL" ON)
if(wxGIS_USE_WXGISGDAL)
    find_package(WXGISGDAL REQUIRED)
else(wxGIS_USE_WXGISGDAL)
    find_package(GDAL REQUIRED)
endif(wxGIS_USE_WXGISGDAL)

if(WXGISGDAL_FOUND)
    include_directories(${WXGISGDAL_INCLUDE_DIR})
    include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/port)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/gcore)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/alg)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    include_directories(${GDAL_INCLUDE_DIR}/ogr)
    include_directories(${GDAL_INCLUDE_DIR}/port)
    include_directories(${GDAL_INCLUDE_DIR}/gcore)
    include_directories(${GDAL_INCLUDE_DIR}/alg)
    include_directories(${GDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
endif(GDAL_FOUND)

find_package( Boost )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DBOOST_ALL_NO_LIB)
endif(Boost_FOUND)

option(wxGIS_USE_POSTGRES "Set OFF to build without PostGIS support" ON)
if(wxGIS_USE_POSTGRES)
    find_package(POSTGRES REQUIRED)
    if(POSTGRES_FOUND)
        include_directories(${POSTGRES_INCLUDE_DIR})
        add_definitions(-DHAVE_PG)
    endif(POSTGRES_FOUND)
endif(wxGIS_USE_POSTGRES)

option(wxGIS_PORTABLE "Set OFF to build non portable version" ON)
if(wxGIS_USE_POSTGRES)
    add_definitions(-DWXGISPORTABLE)
endif(wxGIS_USE_POSTGRES)

if(MSVC)
    #string(REPLACE "/W4" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    #string(REPLACE "/W4" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # string(REPLACE "/Gm" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    #set(CMAKE_CXX_FLAGS_DEBUG "/Gm /ZI /W3 /Od")
    # add_definitions(-DDISABLE_SOME_FLOATING_POINT)
    # set_target_properties( ${the_target} PROPERTIES COMPILE_FLAGS "/Gm" )
    if(CMAKE_CL_64)
        set_target_properties(${the_target} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
    endif()
endif()

if(UNIX)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
     endif()
endif()

#core
set(WXGISCORE_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/core/)
#datasource
set(WXGISDATASOURCE_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/datasource/)
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
#catalog
set(WXGISCATALOG_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/catalog/)
#catalogui
set(WXGISCATALOGUI_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/catalogui/)
#geoprocessing
set(WXGISGEOPROCESSING_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocessing/)
#geoprocessingui
set(WXGISGEOPROCESSINGUI_LIB_NAME "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocessingui/)
#cat_app
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cat_app/)
#geoprocess
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/geoprocess/)

if(WIN32)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/locale CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/locale CONFIGURATIONS Release FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Debug/log)"  CONFIGURATIONS Debug)
install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Release/log)"  CONFIGURATIONS Release)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/sys CONFIGURATIONS Debug)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/sys CONFIGURATIONS Release)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config CONFIGURATIONS Debug)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/config CONFIGURATIONS Release)
endif(WIN32)#in UNIX should install /.../LC...
