cmake_minimum_required (VERSION 2.6)
set(PROJECT_NAME wxgisframework)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/framework)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/framework)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS core base xml html aui adv REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}    
    ${LIB_HEADERS}/accelerator.h
    ${LIB_HEADERS}/addcommanddlg.h
    ${LIB_HEADERS}/animation.h
    ${LIB_HEADERS}/application.h
    ${LIB_HEADERS}/applicationex.h
    ${LIB_HEADERS}/checklist.h
    ${LIB_HEADERS}/command.h
    ${LIB_HEADERS}/commandbar.h
    ${LIB_HEADERS}/createcbdlg.h
    ${LIB_HEADERS}/customizedlg.h
    ${LIB_HEADERS}/droptarget.h
    ${LIB_HEADERS}/framework.h
    ${LIB_HEADERS}/keycodedlg.h
    ${LIB_HEADERS}/menubar.h
    ${LIB_HEADERS}/progressdlg.h
    ${LIB_HEADERS}/progressor.h
    ${LIB_HEADERS}/propertypages.h
    ${LIB_HEADERS}/statusbar.h
    ${LIB_HEADERS}/toolbarmenu.h
    ${LIB_HEADERS}/tooltip.h
    ${LIB_HEADERS}/wxregexvalidator.h
    ${LIB_HEADERS}/xmlnode.h   
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/accelerator.cpp
    ${LIB_SOURCES}/addcommanddlg.cpp
    ${LIB_SOURCES}/animation.cpp
    ${LIB_SOURCES}/application.cpp
    ${LIB_SOURCES}/applicationex.cpp
    ${LIB_SOURCES}/checklist.cpp
    ${LIB_SOURCES}/command.cpp
    ${LIB_SOURCES}/commandbar.cpp
    ${LIB_SOURCES}/createcbdlg.cpp
    ${LIB_SOURCES}/customizedlg.cpp
    ${LIB_SOURCES}/droptarget.cpp
    ${LIB_SOURCES}/keycodedlg.cpp
    ${LIB_SOURCES}/menubar.cpp
    ${LIB_SOURCES}/progressdlg.cpp
    ${LIB_SOURCES}/progressor.cpp
    ${LIB_SOURCES}/propertypages.cpp
    ${LIB_SOURCES}/statusbar.cpp
    ${LIB_SOURCES}/toolbarmenu.cpp
    ${LIB_SOURCES}/tooltip.cpp
    ${LIB_SOURCES}/wxregexvalidator.cpp
    ${LIB_SOURCES}/xmlnode.cpp   
)

add_definitions(-DWXMAKINGDLL_GIS_FRW)
set( WXGISFRAMEWORK_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

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

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME})

include(installlib)
