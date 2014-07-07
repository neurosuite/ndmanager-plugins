project(ndmanager-plugins-src-process_pca)
cmake_minimum_required(VERSION 2.8.6)

# Install program
set(sources process_pca.cpp progressbar.cpp)
set(program process_pca)
add_executable(${program} ${sources})
target_link_libraries(${program} gsl gslcblas m)
install(TARGETS ${program} DESTINATION bin)

# Create and install man pages
set(docbooks process_pca.docbook)
find_file(xsl docbook.xsl /usr/share/apps/ksgmltools2/docbook/xsl/manpages/)
set(manpages)
foreach(docbook ${docbooks})
	string(REGEX REPLACE docbook 1 manpage ${docbook})
	add_custom_command(OUTPUT ${manpage} DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${docbook}" COMMAND xsltproc ${xsl} "${CMAKE_CURRENT_SOURCE_DIR}/${docbook}")
	add_custom_command(OUTPUT ${manpage}.gz DEPENDS ${manpage} COMMAND gzip ${manpage})
	list(APPEND manpages ${manpage}.gz)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${manpage}.gz" DESTINATION man/man1)
endforeach()
add_custom_target(man-${program} ALL DEPENDS ${manpages})
