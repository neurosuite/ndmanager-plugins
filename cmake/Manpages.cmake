# ndm_add_manpages(<target-name> <docbook files...>)
# Builds gzipped section 1 manual pages from DocBook refentry sources and
# installs them. Does nothing when WITH_MANPAGES is off or the tools are missing.
if(WITH_MANPAGES)
    find_program(XSLTPROC_EXECUTABLE xsltproc)
    find_file(DOCBOOK_MANPAGES_XSL docbook.xsl
        PATHS
            /usr/share/xml/docbook/stylesheet/docbook-xsl/manpages
            /usr/share/xml/docbook/stylesheet/nwalsh/current/manpages
            /usr/share/sgml/docbook/xsl-stylesheets/manpages
            /opt/homebrew/opt/docbook-xsl/docbook-xsl/manpages
            /usr/local/opt/docbook-xsl/docbook-xsl/manpages
        PATH_SUFFIXES
            share/xml/docbook-xsl-nons/manpages
            share/xml/docbook-xsl/manpages
        DOC "DocBook XSL stylesheet for manual pages (manpages/docbook.xsl)")
    if(NOT XSLTPROC_EXECUTABLE OR NOT DOCBOOK_MANPAGES_XSL)
        message(STATUS "xsltproc or the DocBook manpage stylesheet not found; manual pages are not built")
        set(WITH_MANPAGES OFF)
    endif()
endif()

function(ndm_add_manpages name)
    if(NOT WITH_MANPAGES)
        return()
    endif()
    set(outputs)
    foreach(docbook ${ARGN})
        get_filename_component(base ${docbook} NAME_WE)
        set(page "${CMAKE_CURRENT_BINARY_DIR}/${base}.1")
        # The sources use &nbsp;, which is only defined by the DocBook DTD.
        # Replace it so that no DTD has to be fetched.
        add_custom_command(OUTPUT ${page}.gz
            COMMAND sed "s/&nbsp;/\\&#160;/g" ${CMAKE_CURRENT_SOURCE_DIR}/${docbook} > ${page}.xml
            COMMAND ${XSLTPROC_EXECUTABLE} --nonet --novalid -o ${page}
                    ${DOCBOOK_MANPAGES_XSL} ${page}.xml
            COMMAND gzip -nf ${page}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${docbook}
            VERBATIM)
        list(APPEND outputs ${page}.gz)
    endforeach()
    add_custom_target(man-${name} ALL DEPENDS ${outputs})
    install(FILES ${outputs} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endfunction()
