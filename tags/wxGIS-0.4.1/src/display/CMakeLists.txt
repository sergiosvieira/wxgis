cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgisdisplay)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/display)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/display)

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

# if(WXGISGDAL_FOUND)
    # include_directories(${WXGISGDAL_INCLUDE_DIR})
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/port)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/gcore)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/alg)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # include_directories(${GDAL_INCLUDE_DIR})
    # include_directories(${GDAL_INCLUDE_DIR}/ogr)
    # include_directories(${GDAL_INCLUDE_DIR}/port)
    # include_directories(${GDAL_INCLUDE_DIR}/gcore)
    # include_directories(${GDAL_INCLUDE_DIR}/alg)
    # include_directories(${GDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
# endif(GDAL_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}    
    ${LIB_HEADERS}/display.h
    ${LIB_HEADERS}/displaytransformation.h
    ${LIB_HEADERS}/gisdisplay.h
    ${LIB_HEADERS}/rubberband.h
    ${LIB_HEADERS}/simplefillsymbol.h
    ${LIB_HEADERS}/simplelinesymbol.h
    ${LIB_HEADERS}/simplemarkersymbol.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/displaytransformation.cpp
    ${LIB_SOURCES}/gisdisplay.cpp
    ${LIB_SOURCES}/rubberband.cpp
    ${LIB_SOURCES}/simplefillsymbol.cpp
    ${LIB_SOURCES}/simplelinesymbol.cpp
    ${LIB_SOURCES}/simplemarkersymbol.cpp    
)

add_definitions(-DWXMAKINGDLL_GIS_DSP)
set( WXGISDISPLAY_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(CAIRO_FOUND)
    target_link_libraries(${LIB_NAME} ${CAIRO_LIBRARIES})
endif(CAIRO_FOUND)

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

target_link_libraries(${LIB_NAME} ${WXGISDATASOURCE_LIB_NAME})

include(installlib)
