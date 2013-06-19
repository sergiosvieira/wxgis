set_target_properties(${LIB_NAME}
	PROPERTIES PROJECT_LABEL ${PROJECT_NAME}
    VERSION ${WXGIS_VERSION}
    SOVERSION 1
	ARCHIVE_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR} 
    )

if(WIN32)
    if(CMAKE_CL_64)
        install(TARGETS ${LIB_NAME} DESTINATION ${WXGIS_CURRENT_SOURCE_DIR}/lib/x64)
    else()
        install(TARGETS ${LIB_NAME} DESTINATION ${WXGIS_CURRENT_SOURCE_DIR}/lib/x32)
     endif()
else()
    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib/wxgis
        LIBRARY DESTINATION lib/wxgis)

    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/wxgis")
endif() 
