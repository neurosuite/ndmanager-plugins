project(ndmanager-src)

set(ndmanager_SRCS
	acquisitionsystempage.cpp
        anatomypage.cpp 
	channelcolors.cpp 
        channelcolorspage.cpp 
	channeloffsetspage.cpp 
        clusterspage.cpp 
	configuration.cpp 
	descriptionwriter.cpp 
        fileinformation.cpp 
	filepage.cpp 
	filespage.cpp 
        generalinfopage.cpp 
	generalinformation.cpp 
	helper.cpp 
	launcherpage.cpp
        lfppage.cpp 
	main.cpp 
	managerview.cpp 
	miscellaneouspage.cpp
	ndmanager.cpp 
	ndmanagerdoc.cpp
	neuroscopevideoinfo.cpp 
	neuroscopevideopage.cpp
        parameterpage.cpp 
	parameterview.cpp 
	programinformation.cpp 
	programspage.cpp
	programpage.cpp	
	queryinputdialog.cpp 
	queryoutputdialog.cpp 
	spikepage.cpp
        tags.cpp 
	unitlistlayout.cpp 
	unitlistpage.cpp 
	unittable.cpp 
	unittableitem.cpp
        videopage.cpp 
	xmlreader.cpp 
	xmlwriter.cpp 
	acquisitionsystemlayout.cpp
	anatomylayout.cpp
	channelcolorslayout.cpp
	channeloffsetslayout.cpp
	clusterslayout.cpp
	filelayout.cpp
	generalinfolayout.cpp
	launcherlayout.cpp
	lfplayout.cpp
	neuroscopemisclayout.cpp
	neuroscopevideolayout.cpp
	parameterlayout.cpp
	queryinputdialogbase.cpp
	spikelayout.cpp
	videolayout.cpp
	ndmanagerutils.cpp
	)

qt4_add_resources(ndmanager_SRCS ndmanager-icons.qrc ndmanager-html.qrc)

set(ndmanager_UI 
	acquisitionsystemlayout.ui 
	anatomylayout.ui
	channelcolorslayout.ui
	channeloffsetslayout.ui
	clusterslayout.ui
       	filelayout.ui
	generalinfolayout.ui
	launcherlayout.ui
	lfplayout.ui
	neuroscopemisclayout.ui
	neuroscopevideolayout.ui
	parameterlayout.ui
        queryinputdialogbase.ui
	spikelayout.ui 
	videolayout.ui ) 

qt4_wrap_ui(ndmanager_SRCS ${ndmanager_UI})

add_executable(ndmanager ${ndmanager_SRCS} )

target_link_libraries(ndmanager ${QT_LIBRARIES} ${LIBXML2_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${LIBKLUSTERSSHARED_LIBRARY})

if(QT_QTWEBKIT_FOUND)
	target_link_libraries(ndmanager ${QT_QTWEBKIT_LIBRARY})
endif()

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/ndmanager DESTINATION bin)
