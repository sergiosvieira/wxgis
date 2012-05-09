cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgisgeoprocess)
set(APP_NAME wxgisgeoprocess)
set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/geoprocess.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/geoprocess.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/geoprocess)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/geoprocess)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

# find_package(GEOS REQUIRED)
# if(GEOS_FOUND)
    # #get GEOS root directory from GEOS_INCLUDE_DIR
    # get_filename_component(GEOS_ROOT ${GEOS_INCLUDE_DIR} PATH)
    # include_directories(${GEOS_INCLUDE_DIR})
    # include_directories(${GEOS_ROOT}/capi)
    # add_definitions(-DHAVE_GEOS)
# endif(GEOS_FOUND)

find_package(PROJ REQUIRED)
if(PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    add_definitions(-DHAVE_PROJ -DPROJ_STATIC)
endif(PROJ_FOUND)

# if(WXGISGDAL_FOUND)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(GDAL_FOUND)

include_directories(${APP_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${APP_HEADERS}/geoprocess.h
    ${APP_HEADERS}/processmain.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/processmain.cpp
)

if(WIN32)
    add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})
else(WIN32)
    add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})
endif(WIN32)

if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

# if(WXGISGDAL_FOUND)
    # target_link_libraries(${APP_NAME} ${WXGISGDAL_LIBRARIES})
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # target_link_libraries(${APP_NAME} ${GDAL_LIBRARIES})
# endif(GDAL_FOUND) 

if(PROJ_FOUND)
    target_link_libraries(${APP_NAME} ${PROJ_LIBRARY})
endif(PROJ_FOUND)

# if(GEOS_FOUND)
    # target_link_libraries(${APP_NAME} ${GEOS_LIBRARY})
# endif(GEOS_FOUND)

# if(CAIRO_FOUND)
    # target_link_libraries(${APP_NAME} ${CAIRO_LIBRARIES})
# endif(CAIRO_FOUND)

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME} ${WXGISGEOPROCESSING_LIB_NAME} )

include(installapp)

