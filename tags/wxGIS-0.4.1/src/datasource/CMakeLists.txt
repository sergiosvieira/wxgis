cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgisdatasource)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/datasource)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/datasource)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base xml REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

find_package(CURL REQUIRED)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif(CURL_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}    
    ${LIB_HEADERS}/curl.h
    ${LIB_HEADERS}/cursor.h
    ${LIB_HEADERS}/datacontainer.h
    ${LIB_HEADERS}/datasource.h
    ${LIB_HEADERS}/featuredataset.h
    ${LIB_HEADERS}/filter.h
    ${LIB_HEADERS}/postgisdataset.h
    ${LIB_HEADERS}/quadtree.h
    ${LIB_HEADERS}/rasterdataset.h
    ${LIB_HEADERS}/rasterop.h
    ${LIB_HEADERS}/spvalidator.h
    ${LIB_HEADERS}/sysop.h
    ${LIB_HEADERS}/table.h
    ${LIB_HEADERS}/vectorop.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/curl.cpp
    ${LIB_SOURCES}/cursor.cpp
    ${LIB_SOURCES}/datacontainer.cpp
    ${LIB_SOURCES}/featuredataset.cpp
    ${LIB_SOURCES}/filter.cpp
    ${LIB_SOURCES}/postgisdataset.cpp
    ${LIB_SOURCES}/quadtree.cpp
    ${LIB_SOURCES}/rasterdataset.cpp
    ${LIB_SOURCES}/rasterop.cpp
    ${LIB_SOURCES}/spvalidator.cpp
    ${LIB_SOURCES}/sysop.cpp
    ${LIB_SOURCES}/table.cpp
    ${LIB_SOURCES}/vectorop.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_DS)
set( WXGISDATASOURCE_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(CURL_FOUND)
    target_link_libraries(${LIB_NAME} ${CURL_LIBRARIES})
endif(CURL_FOUND)

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

include(installlib)
