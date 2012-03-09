cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgisgeoprocessing)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/geoprocessing)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/geoprocessing)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

if(WXGISGDAL_FOUND)
    include_directories(${WXGISGDAL_INCLUDE_DIR}/frmts)
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR}/frmts)
endif(GDAL_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS} 
    ${LIB_HEADERS}/geoprocessing.h
    ${LIB_HEADERS}/gpcompstattool.h
    ${LIB_HEADERS}/gpcreateovrtool.h
    ${LIB_HEADERS}/gpdomain.h
    ${LIB_HEADERS}/gpexporttool.h
    ${LIB_HEADERS}/gpgeomverttoptstool.h
    ${LIB_HEADERS}/gporthocorrecttool.h
    ${LIB_HEADERS}/gpparam.h
    ${LIB_HEADERS}/gpprojecttool.h
    ${LIB_HEADERS}/gpraster.h
    ${LIB_HEADERS}/gpshapetotexttool.h
    ${LIB_HEADERS}/gpsubrasterbyvectortool.h
    ${LIB_HEADERS}/gpsumtabletool.h
    ${LIB_HEADERS}/gptable.h
    ${LIB_HEADERS}/gptool.h
    ${LIB_HEADERS}/gptoolmngr.h
    ${LIB_HEADERS}/gpvector.h    
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}    
    ${LIB_SOURCES}/gpcompstattool.cpp
    ${LIB_SOURCES}/gpcreateovrtool.cpp
    ${LIB_SOURCES}/gpdomain.cpp
    ${LIB_SOURCES}/gpexporttool.cpp
    ${LIB_SOURCES}/gpgeomverttoptstool.cpp
    ${LIB_SOURCES}/gporthocorrecttool.cpp
    ${LIB_SOURCES}/gpparam.cpp
    ${LIB_SOURCES}/gpprojecttool.cpp
    ${LIB_SOURCES}/gpraster.cpp
    ${LIB_SOURCES}/gpshapetotexttool.cpp
    ${LIB_SOURCES}/gpsubrasterbyvectortool.cpp
    ${LIB_SOURCES}/gpsumtabletool.cpp
    ${LIB_SOURCES}/gptable.cpp
    ${LIB_SOURCES}/gptool.cpp
    ${LIB_SOURCES}/gptoolmngr.cpp
    ${LIB_SOURCES}/gpvector.cpp    
)

add_definitions(-DWXMAKINGDLL_GIS_GP)
set( WXGISGEOPROCESSING_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

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

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME}  ${WXGISCATALOG_LIB_NAME})

include(installlib)