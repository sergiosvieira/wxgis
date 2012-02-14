cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgiscarto)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/carto)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/carto)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

find_package(CAIRO REQUIRED)
if(CAIRO_FOUND)
    include_directories(${CAIRO_INCLUDE_DIR})
    add_definitions(-DHAVE_CAIRO)
endif(CAIRO_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${LIB_HEADERS}/carto.h
    ${LIB_HEADERS}/featurelayer.h
    ${LIB_HEADERS}/interpolation.h
    ${LIB_HEADERS}/map.h
    ${LIB_HEADERS}/rasterlayer.h
    ${LIB_HEADERS}/rasterrenderer.h
    ${LIB_HEADERS}/simplerenderer.h
    ${LIB_HEADERS}/stretch.h
    ${LIB_HEADERS}/transformthreads.h    
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/featurelayer.cpp
    ${LIB_SOURCES}/interpolation.cpp
    ${LIB_SOURCES}/map.cpp
    ${LIB_SOURCES}/rasterlayer.cpp
    ${LIB_SOURCES}/rasterrenderer.cpp
    ${LIB_SOURCES}/simplerenderer.cpp
    ${LIB_SOURCES}/stretch.cpp
    ${LIB_SOURCES}/transformthreads.cpp    
)

add_definitions(-DWXMAKINGDLL_GIS_CRT)
set( WXGISCARTO_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(CAIRO_FOUND)
    target_link_libraries(${LIB_NAME} ${CAIRO_LIBRARIES})
endif(CAIRO_FOUND)

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME} ${WXGISFRAMEWORK_LIB_NAME} ${WXGISDISPLAY_LIB_NAME})

include(installlib)