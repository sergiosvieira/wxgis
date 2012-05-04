cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgiscore)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/core)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/core)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base xml REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

find_package( OpenSSL )
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif(OPENSSL_FOUND) 

find_package(PROJ REQUIRED)
if(PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    add_definitions(-DHAVE_PROJ -DPROJ_STATIC)
endif(PROJ_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/config.h
    ${LIB_HEADERS}/core.h
    ${LIB_HEADERS}/crypt.h
    ${LIB_HEADERS}/event.h
    ${LIB_HEADERS}/format.h
    ${LIB_HEADERS}/globalfn.h
    ${LIB_HEADERS}/init.h
    ${LIB_HEADERS}/process.h
    ${LIB_HEADERS}/process.hpp
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/config.cpp
    ${LIB_SOURCES}/crypt.cpp
    ${LIB_SOURCES}/event.cpp
    ${LIB_SOURCES}/format.cpp
    ${LIB_SOURCES}/globalfn.cpp
    ${LIB_SOURCES}/init.cpp
    ${LIB_SOURCES}/process.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_CORE)
set(WXGISCORE_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(OPENSSL_FOUND)
    target_link_libraries(${LIB_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND) 

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(PROJ_FOUND)
    target_link_libraries(${LIB_NAME} ${PROJ_LIBRARY})
endif(PROJ_FOUND)

include(installlib)
