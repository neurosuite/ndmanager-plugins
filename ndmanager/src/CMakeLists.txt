project(ndmanager-src)

set(ndmanager_SRCS acquisitionsystempage.cpp
        anatomypage.cpp channelcolors.cpp 
        channelcolorspage.cpp channeloffsetspage.cpp 
        clusterspage.cpp configuration.cpp descriptionwriter.cpp editormanager.cpp
        fileinformation.cpp filepage.cpp filespage.cpp 
        generalinfopage.cpp generalinformation.cpp helper.cpp launcherpage.cpp
        lfppage.cpp main.cpp managerview.cpp miscellaneouspage.cpp
        ndkonsole.cpp ndmanager.cpp ndmanagerdoc.cpp
        neuroscopevideoinfo.cpp neuroscopevideopage.cpp
        parameterpage.cpp parameterview.cpp pref.cpp programinformation.cpp programpage.cpp
        programspage.cpp queryinputdialog.cpp queryoutputdialog.cpp spikepage.cpp
        tags.cpp unitlistlayout.cpp unitlistpage.cpp unittable.cpp unittableitem.cpp
        videopage.cpp xmlreader.cpp xmlwriter.cpp)

kde3_automoc(${ndmanager_SRCS})

set(ndmanager_UI acquisitionsystemlayout.ui anatomylayout.ui channelcolorslayout.ui channeloffsetslayout.ui clusterslayout.ui filelayout.ui generalinfolayout.ui launcherlayout.ui lfplayout.ui neuroscopemisclayout.ui neuroscopevideolayout.ui parameterlayout.ui spikelayout.ui videolayout.ui ) 

KDE3_ADD_UI_FILES(ndmanager_SRCS ${ndmanager_UI})

add_executable(ndmanager ${ndmanager_SRCS} )

target_link_libraries(ndmanager ${QT_LIBRARIES} kdeui kdecore kio kdeprint kdefx kparts ktexteditor ${LIBXML2_LIBRARIES})

