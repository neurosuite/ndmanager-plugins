project(ndmanager-plugins-python-ndm_checkconsistency)
cmake_minimum_required(VERSION 2.8.6)

find_package(KDE4 4.0.0 REQUIRED)
include(KDE4Defaults)
find_package(PyQt4 4.0.0 REQUIRED)

# Make variables
set(header header.py)
set(source main.py)
set(ui Ui_InfoDialog.ui)
set(ressources ressources.qrc)
set(images images/binary.png images/cancel.png images/ndmanager.png images/ok.png images/video.png)
set(ressourcesc ressources.py)
set(program ndm_checkconsistency)
set(desktop ndm_checkconsistency_kde4.desktop)

# List images (prepend source dir)
set(list)
foreach(i ${images})
	list(APPEND list "${CMAKE_CURRENT_SOURCE_DIR}/${i}")
endforeach()
set(images ${list})

# Build ressources
add_custom_command(OUTPUT ${ressourcesc} DEPENDS ${images} "${CMAKE_CURRENT_SOURCE_DIR}/${ressources}" COMMAND pyrcc4 -o ${ressourcesc} "${CMAKE_CURRENT_SOURCE_DIR}/${ressources}")

# Build user interface (ui) files
set(uic)
foreach(i ${ui})
	string(REGEX REPLACE ui py j ${i})
	add_custom_command(OUTPUT ${j} DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${i}" COMMAND pyuic4 -o ${j} "${CMAKE_CURRENT_SOURCE_DIR}/${i}")
	list(APPEND uic ${j})
endforeach()
add_custom_target(${program}-ressources ALL DEPENDS ${uic})

# Concatenate all files to create final program
add_custom_command(OUTPUT full DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${header}" "${CMAKE_CURRENT_SOURCE_DIR}/${source}" ${ressourcesc} ${uic} COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/${header}" ${uic} ${ressourcesc} "${CMAKE_CURRENT_SOURCE_DIR}/${source}" | grep -v ressources_rc > ${program})

# Install program
add_custom_target(${program} ALL DEPENDS full)
install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${program}" DESTINATION bin)

# Install service menus
install(FILES ${desktop} DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)

# Create and install man pages
set(docbooks ndm_checkconsistency.docbook)
file(GLOB_RECURSE files /usr/share/docbook.xsl)
foreach(file ${files})
	string(FIND ${file} manpages found)
	if (NOT found EQUAL -1)
		set(xsl ${file})
	endif()
endforeach()
set(manpages)
foreach(docbook ${docbooks})
	string(REGEX REPLACE docbook 1 manpage ${docbook})
	add_custom_command(OUTPUT ${manpage} DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${docbook}" COMMAND xsltproc ${xsl} "${CMAKE_CURRENT_SOURCE_DIR}/${docbook}")
	add_custom_command(OUTPUT ${manpage}.gz DEPENDS ${manpage} COMMAND gzip ${manpage})
	list(APPEND manpages ${manpage}.gz)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${manpage}.gz" DESTINATION man/man1)
endforeach()
add_custom_target(man-${program} ALL DEPENDS ${manpages})
