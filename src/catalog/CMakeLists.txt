cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgiscatalog)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/catalog)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/catalog)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

find_package( OpenSSL )
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif(OPENSSL_FOUND)

if(WXGISGDAL_FOUND)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
endif(GDAL_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${LIB_HEADERS}/catalog.h
    ${LIB_HEADERS}/catop.h
    ${LIB_HEADERS}/gxarchfolder.h
    ${LIB_HEADERS}/gxarchivefactory.h
    ${LIB_HEADERS}/gxcatalog.h
    ${LIB_HEADERS}/gxcsvfactory.h
    ${LIB_HEADERS}/gxdataset.h
    ${LIB_HEADERS}/gxdiscconnection.h
    ${LIB_HEADERS}/gxdiscconnections.h
    ${LIB_HEADERS}/gxevent.h
    ${LIB_HEADERS}/gxfile.h
    ${LIB_HEADERS}/gxfilefactory.h
    ${LIB_HEADERS}/gxfilters.h
    ${LIB_HEADERS}/gxfolder.h
    ${LIB_HEADERS}/gxfolderfactory.h
    ${LIB_HEADERS}/gxkmldataset.h
    ${LIB_HEADERS}/gxmapinfofactory.h
    ${LIB_HEADERS}/gxmlfactory.h
    ${LIB_HEADERS}/gxprjfactory.h
    ${LIB_HEADERS}/gxrasterfactory.h
    ${LIB_HEADERS}/gxshapefactory.h
    ${LIB_HEADERS}/gxspatreffolder.h
)

if(POSTGRES_FOUND)
set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/gxpostgisdataset.h
    ${LIB_HEADERS}/gxdbconnections.h
    ${LIB_HEADERS}/gxdbconnfactory.h
    ${LIB_HEADERS}/gxremoteconn.h
    )
endif(POSTGRES_FOUND)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/catop.cpp
    ${LIB_SOURCES}/gxarchfolder.cpp
    ${LIB_SOURCES}/gxarchivefactory.cpp
    ${LIB_SOURCES}/gxcatalog.cpp
    ${LIB_SOURCES}/gxcsvfactory.cpp
    ${LIB_SOURCES}/gxdataset.cpp
    ${LIB_SOURCES}/gxdiscconnection.cpp
    ${LIB_SOURCES}/gxdiscconnections.cpp
    ${LIB_SOURCES}/gxevent.cpp
    ${LIB_SOURCES}/gxfile.cpp
    ${LIB_SOURCES}/gxfilefactory.cpp
    ${LIB_SOURCES}/gxfilters.cpp
    ${LIB_SOURCES}/gxfolder.cpp
    ${LIB_SOURCES}/gxfolderfactory.cpp
    ${LIB_SOURCES}/gxkmldataset.cpp
    ${LIB_SOURCES}/gxmapinfofactory.cpp
    ${LIB_SOURCES}/gxmlfactory.cpp
    ${LIB_SOURCES}/gxprjfactory.cpp
    ${LIB_SOURCES}/gxrasterfactory.cpp
    ${LIB_SOURCES}/gxshapefactory.cpp
    ${LIB_SOURCES}/gxspatreffolder.cpp    
)

if(POSTGRES_FOUND)
set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/gxpostgisdataset.cpp
    ${LIB_SOURCES}/gxdbconnections.cpp
    ${LIB_SOURCES}/gxdbconnfactory.cpp
    ${LIB_SOURCES}/gxremoteconn.cpp
    )
endif(POSTGRES_FOUND)
 
add_definitions(-DWXMAKINGDLL_GIS_CLT)
set( WXGISCATALOG_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

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

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME})

include(installlib)
