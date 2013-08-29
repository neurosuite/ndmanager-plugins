project(ndmanager-src)

set(ndmanager_SRCS
	acquisitionsystempage.cpp
        anatomypage.cpp 
	channelcolors.cpp 
        channelcolorspage.cpp 
	channeloffsetspage.cpp 
        clusterspage.cpp 
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
        colortable.cpp
        parametertree.cpp
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

add_executable(ndmanager WIN32 ${ndmanager_SRCS} )


if(Qt5Core_FOUND)
  target_link_libraries( ndmanager ${LIBKLUSTERSSHARED_LIBRARY} Qt5::Widgets Qt5::Xml ${Qt5WebKitWidgets_LIBRARIES})
else(Qt5Core_FOUND)
  target_link_libraries( ndmanager ${LIBKLUSTERSSHARED_LIBRARY} ${QT_LIBRARIES} ${QT_QTXML_LIBRARY})
if(QT_QTWEBKIT_FOUND)
    target_link_libraries(ndmanager ${QT_QTWEBKIT_LIBRARY})
endif()

endif()



install(FILES ndManagerDefault.xml DESTINATION share/applications/ndmanager/)

install(TARGETS ndmanager DESTINATION bin)

install(FILES hi16-app-ndmanager.png DESTINATION share/icons/hicolor/16x16/apps/ RENAME ndmanager.png)
install(FILES hi22-app-ndmanager.png DESTINATION share/icons/hicolor/22x22/apps/ RENAME ndmanager.png)
install(FILES hi32-app-ndmanager.png DESTINATION share/icons/hicolor/32x32/apps/ RENAME ndmanager.png)
install(FILES hi48-app-ndmanager.png DESTINATION share/icons/hicolor/48x48/apps/ RENAME ndmanager.png)
install(FILES hi64-app-ndmanager.png DESTINATION share/icons/hicolor/64x64/apps/ RENAME ndmanager.png)

install(FILES hi16-nphys-xml.png DESTINATION share/icons/hicolor/16x16/mimetypes/ RENAME application-nphys-xml.png)
install(FILES hi22-nphys-xml.png DESTINATION share/icons/hicolor/22x22/mimetypes/ RENAME application-nphys-xml.png)
install(FILES hi32-nphys-xml.png DESTINATION share/icons/hicolor/32x32/mimetypes/ RENAME application-nphys-xml.png)
install(FILES hi48-nphys-xml.png DESTINATION share/icons/hicolor/48x48/mimetypes/ RENAME application-nphys-xml.png)
install(FILES hi64-nphys-xml.png DESTINATION share/icons/hicolor/64x64/mimetypes/ RENAME application-nphys-xml.png)

install(FILES ndmanager.desktop DESTINATION share/applications/ )
install(FILES ndmanager.xml DESTINATION share/mime/packages/)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/Template*.xml")
install(FILES ${files} DESTINATION share/ndmanager/templates/)

