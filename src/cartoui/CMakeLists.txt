cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgiscartoui)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/cartoui)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/cartoui)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base propgrid REQUIRED)
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
    ${LIB_HEADERS}/cartocmd.h
    ${LIB_HEADERS}/cartoui.h
    ${LIB_HEADERS}/formatmenu.h
    ${LIB_HEADERS}/identifydlg.h
    ${LIB_HEADERS}/mapview.h
    ${LIB_HEADERS}/mxeventui.h
    ${LIB_HEADERS}/rasterdatasetcmd.h
    ${LIB_HEADERS}/sqlquerydialog.h
    ${LIB_HEADERS}/tableview.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/cartocmd.cpp
    ${LIB_SOURCES}/formatmenu.cpp
    ${LIB_SOURCES}/identifydlg.cpp
    ${LIB_SOURCES}/mapview.cpp
    ${LIB_SOURCES}/mxeventui.cpp
    ${LIB_SOURCES}/rasterdatasetcmd.cpp
    ${LIB_SOURCES}/sqlquerydialog.cpp
    ${LIB_SOURCES}/tableview.cpp    
)

add_definitions(-DWXMAKINGDLL_GIS_CTU)
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

target_link_libraries(${LIB_NAME} ${WXGISCARTO_LIB_NAME})

include(installlib)
