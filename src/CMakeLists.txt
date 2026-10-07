find_package(GSL REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SAMPLERATE REQUIRED IMPORTED_TARGET samplerate)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavformat libavcodec libswscale libavutil)

function(ndm_tool name)
    cmake_parse_arguments(T "" "" "SOURCES;LIBS" ${ARGN})
    list(TRANSFORM T_SOURCES PREPEND "${name}/")
    add_executable(${name} ${T_SOURCES})
    target_link_libraries(${name} PRIVATE ${T_LIBS})
    install(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    ndm_add_manpages(${name} ${name}/${name}.docbook)
endfunction()

ndm_tool(process_extractchannels SOURCES process_extractchannels.cpp)
ndm_tool(process_extractleds     SOURCES process_extractleds.c LIBS PkgConfig::FFMPEG m)
ndm_tool(process_extractspikes   SOURCES process_extractspikes.cpp)
ndm_tool(process_medianfilter    SOURCES process_medianfilter.cpp)
ndm_tool(process_medianthreshold SOURCES process_medianthreshold.cpp)
ndm_tool(process_merge           SOURCES process_merge.cpp)
ndm_tool(process_nlxconvert      SOURCES
    ncsfilecollection.cpp ncsfile.cpp nevfile.cpp nlxerror.cpp nlxextractor.cpp
    nvtfile.cpp outputfile.cpp process_nlxconvert.cpp progressbar.cpp
    smifilecollection.cpp smifile.cpp)
ndm_tool(process_pca             SOURCES process_pca.cpp progressbar.cpp LIBS GSL::gsl GSL::gslcblas)
ndm_tool(process_removeartefacts SOURCES process_removeartefacts.cpp)
ndm_tool(process_resample        SOURCES process_resample.c LIBS PkgConfig::SAMPLERATE m)
ndm_tool(process_smrconvert      SOURCES
    process_smrconvert.cpp sonadcchannels.cpp sonadcmarkchannel.cpp sonblockheaders.cpp
    sonchannel.cpp sonchannelinfo.cpp sonerror.cpp soneventchannel.cpp sonextractor.cpp
    sonfile.cpp sonfileheader.cpp)
ndm_tool(xpathReader             SOURCES xpathReader.c LIBS LibXml2::LibXml2)

# 'process' is documentation only (overview of the process_* tools)
ndm_add_manpages(process process/process.docbook)
