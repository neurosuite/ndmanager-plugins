project(ndmanager-plugins-scripts)
cmake_minimum_required(VERSION 2.8.6)

# Install scripts
# Create list with 'ls|grep -v CMakeLists.txt|grep -v docbook'
set(program ndm)
set(scripts
	cnt
	duration
	ndm_clean
	ndm_concatenate
	ndm_extractchannels
	ndm_extractleds
	ndm_extractspikes
	ndm_functions
	ndm_hipass
	ndm_lfp
	ndm_mergedat
	ndm_ncs2dat
	ndm_nev2evt
	ndm_nvt2spots
	ndm_pca
	ndm_recolorchannels
	ndm_reorderchannels
	ndm_resample
	ndm_smi2sts
	ndm_smr2dat
	ndm_smr2evt
	ndm_start
	ndm_transcodevideo
	pad
	process_renameevents_SPWR
	process_renameevents_TMaze
	process_reorderclusters
	rn)
install(PROGRAMS ${scripts} DESTINATION bin)

# Create and install man pages
set(docbooks
	cnt.docbook
	duration.docbook
	ndm.docbook
	ndm_recolorchannels.docbook
	ndm_start.docbook
	pad.docbook
	process_reorderclusters.docbook
	rn.docbook)
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
