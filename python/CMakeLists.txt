# The Python tools are assembled into single executable scripts:
# header.py + the pyuic6-generated dialogs + main.py.
find_program(PYUIC6_EXECUTABLE pyuic6 REQUIRED)

set(PYTHON_IMAGES_DIR ${CMAKE_INSTALL_DATADIR}/ndmanager-plugins/images)

function(ndm_python_tool name)
    cmake_parse_arguments(T "" "" "UI;IMAGES" ${ARGN})
    set(dir ${CMAKE_CURRENT_SOURCE_DIR}/${name})
    set(generated)
    foreach(ui ${T_UI})
        get_filename_component(base ${ui} NAME_WE)
        set(py ${CMAKE_CURRENT_BINARY_DIR}/${name}-${base}.py)
        # Icons come from the installed images directory ('images:' search path),
        # since PyQt6 has no resource compiler.
        add_custom_command(OUTPUT ${py}
            COMMAND ${PYUIC6_EXECUTABLE} -o ${py}.in ${dir}/${ui}
            COMMAND sed -e "s|:/images/|images:|g" -e "/ressources_rc/d" ${py}.in > ${py}
            DEPENDS ${dir}/${ui}
            VERBATIM)
        list(APPEND generated ${py})
    endforeach()
    set(program ${CMAKE_CURRENT_BINARY_DIR}/${name})
    add_custom_command(OUTPUT ${program}
        COMMAND cat ${dir}/header.py ${generated} ${dir}/main.py > ${program}
        COMMAND chmod +x ${program}
        DEPENDS ${dir}/header.py ${dir}/main.py ${generated}
        VERBATIM)
    add_custom_target(${name}-script ALL DEPENDS ${program})
    install(PROGRAMS ${program} DESTINATION ${CMAKE_INSTALL_BINDIR})
    list(TRANSFORM T_IMAGES PREPEND "${dir}/")
    install(FILES ${T_IMAGES} DESTINATION ${PYTHON_IMAGES_DIR})
    ndm_add_manpages(${name} ${name}/${name}.docbook)
endfunction()

ndm_python_tool(ndm_prepare
    UI Ui_InputDialog.ui Ui_RenameDialog.ui
    IMAGES images/binary.png images/ndmanager.png images/video.png)
ndm_python_tool(ndm_checkconsistency
    UI Ui_InfoDialog.ui
    IMAGES images/cancel.png images/ok.png images/ndmanager.png images/video.png images/binary.png)
