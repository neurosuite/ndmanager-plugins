file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
install(FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/ndmanager/descriptions)
