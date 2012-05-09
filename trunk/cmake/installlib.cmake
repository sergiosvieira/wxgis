set_target_properties(${LIB_NAME} PROPERTIES PROJECT_LABEL ${PROJECT_NAME})
set_target_properties(${LIB_NAME} PROPERTIES VERSION ${WXGIS_VERSION})
set_target_properties(${LIB_NAME} PROPERTIES SOVERSION 1)

#add_custom_target(uninstall
#    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(WIN32)
    install(TARGETS ${LIB_NAME} DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/ CONFIGURATIONS Debug)
    install(TARGETS ${LIB_NAME} DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/ CONFIGURATIONS Release)
else()
    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION bin/wxgis
        ARCHIVE DESTINATION lib/wxgis
        LIBRARY DESTINATION lib/wxgis )
endif() 

