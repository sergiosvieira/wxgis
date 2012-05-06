cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgiscatalogapp)
set(APP_NAME wxgiscatalog)
set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/catalog.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/catalog_x.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/cat_app)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/cat_app)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
#    if(UNIX)
#      add_definitions("-D${wxWidgets_DEFINITIONS}") #wxWidgets_DEFINITIONS_DEBUG
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
#    endif(UNIX)
endif(wxWidgets_FOUND)

find_package(GEOS REQUIRED)
if(GEOS_FOUND)
    #get GEOS root directory from GEOS_INCLUDE_DIR
    get_filename_component(GEOS_ROOT ${GEOS_INCLUDE_DIR} PATH)
    include_directories(${GEOS_INCLUDE_DIR})
    include_directories(${GEOS_ROOT}/capi)
    add_definitions(-DHAVE_GEOS)
endif(GEOS_FOUND)

find_package(PROJ REQUIRED)
if(PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    add_definitions(-DHAVE_PROJ -DPROJ_STATIC)
endif(PROJ_FOUND)

find_package(CAIRO REQUIRED)
if(CAIRO_FOUND)
    include_directories(${CAIRO_INCLUDE_DIR})
    add_definitions(-DHAVE_CAIRO)
endif(CAIRO_FOUND)

# if(WXGISGDAL_FOUND)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(GDAL_FOUND)



include_directories(${APP_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${APP_HEADERS}/catalogapp.h
    ${APP_HEADERS}/catalogframe.h
    ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/app/gisaboutdlg.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/catalogapp.cpp
    ${APP_SOURCES}/catalogframe.cpp
    ${WXGIS_CURRENT_SOURCE_DIR}/src/app/gisaboutdlg.cpp
)

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})
else(WIN32)
    add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})
endif(WIN32)

if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(WXGISGDAL_FOUND)
    target_link_libraries(${APP_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${APP_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(PROJ_FOUND)
    target_link_libraries(${APP_NAME} ${PROJ_LIBRARY})
endif(PROJ_FOUND)
if(GEOS_FOUND)
    target_link_libraries(${APP_NAME} ${GEOS_LIBRARY})
endif(GEOS_FOUND)

if(CAIRO_FOUND)
    target_link_libraries(${APP_NAME} ${CAIRO_LIBRARIES})
endif(CAIRO_FOUND)

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME} ${WXGISFRAMEWORK_LIB_NAME} ${WXGISCATALOGUI_LIB_NAME} )

set_target_properties(${APP_NAME} PROPERTIES PROJECT_LABEL ${PROJECT_NAME})
set_target_properties(${APP_NAME} PROPERTIES VERSION ${WXGIS_VERSION})
set_target_properties(${APP_NAME} PROPERTIES SOVERSION 1)

install(TARGETS ${APP_NAME} DESTINATION ${APP_INSTALL_PATH_DEBUG} CONFIGURATIONS Debug)
install(TARGETS ${APP_NAME} DESTINATION ${APP_INSTALL_PATH_RELEASE} CONFIGURATIONS Release)
if(NOT WIN32)
    install(FILES PROJECT_HHEADERS DESTINATION ${HEADERS_INSTALL_PATH})
endif(NOT WIN32)

