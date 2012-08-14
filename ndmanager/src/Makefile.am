## Makefile.am for ndmanager

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ndmanager

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src/icons $(LIBXML_CFLAGS) $(all_includes)

# the library search path.
ndmanager_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(LIBXML_LIBS)

# the libraries to link against.
ndmanager_LDADD = -lkhtml -lktexteditor $(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for ndmanager


# these are the headers for your project


# client stuff

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ndmanager.pot

KDE_ICON = AUTO

# this is where the kdelnk file will go
kdelnkdir = $(kde_appsdir)/Science


# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/ndmanager

# this is where the template files go
tmpldir = /usr/share/ndmanager/templates
tmpl_DATA = Template-16-tetrodes-20000Hz.xml Template-16-tetrodes-32552Hz.xml Template-1-tetrode-20000Hz.xml Template-1-tetrode-32552Hz.xml Template-2-tetrodes-20000Hz.xml Template-2-tetrodes-32552Hz.xml Template-3-tetrodes-20000Hz.xml Template-3-tetrodes-32552Hz.xml Template-4-octrodes-20000Hz.xml Template-4-octrodes-32552Hz.xml Template-4-tetrodes-20000Hz.xml Template-4-tetrodes-32552Hz.xml Template-8-octrodes-20000Hz.xml Template-8-octrodes-32552Hz.xml Template-8-tetrodes-20000Hz.xml Template-8-tetrodes-32552Hz.xml

kdelnk_DATA = ndmanager.desktop
rc_DATA = ndManagerDefault.xml ndmanager_katepartui.rc ndmanagerui.rc \
	queryreport.html
noinst_HEADERS = acquisitionsystempage.h anatomypage.h channelcolors.h \
	channelcolors.h	channelcolorspage.h channelcolorspage.h channeloffsetspage.h clusterspage.h configuration.h \
	configuration.h	descriptionwriter.h descriptionwriter.h editormanager.h fileinformation.h filepage.h \
	filepage.h	filespage.h filespage.h generalinfopage.h generalinformation.h helper.h \
	launcherpage.h launcherpage.h	lfppage.h lfppage.h managerview.h miscellaneouspage.h \
	ndkonsole.h ndmanager.h ndmanagerdoc.h ndmanagerdoc.h	ndmanager.h \
	neuroscopevideoinfo.h neuroscopevideopage.h parameterpage.h parameterpage.h	parameterview.h \
	parameterview.h pref.h programinformation.h programpage.h programspage.h \
	programspage.h	spikepage.h queryinputdialog.h queryoutputdialog.h spikepage.h tags.h \
	unitlistpage.h unittable.h unittableitem.h videopage.h xmlreader.h xmlwriter.h
ndmanager_SOURCES = acquisitionsystemlayout.ui acquisitionsystempage.cpp \
	anatomylayout.ui anatomypage.cpp channelcolors.cpp channelcolorslayout.ui \
	channelcolorspage.cpp channeloffsetslayout.ui channeloffsetspage.cpp clusterslayout.ui \
	clusterspage.cpp configuration.cpp descriptionwriter.cpp editormanager.cpp \
	fileinformation.cpp filelayout.ui filepage.cpp filespage.cpp generalinfolayout.ui \
	generalinfopage.cpp generalinformation.cpp helper.cpp launcherlayout.ui launcherpage.cpp \
	lfplayout.ui lfppage.cpp main.cpp managerview.cpp miscellaneouspage.cpp \
	ndkonsole.cpp ndmanager.cpp ndmanagerdoc.cpp neuroscopemisclayout.ui \
	neuroscopevideoinfo.cpp neuroscopevideolayout.ui neuroscopevideopage.cpp parameterlayout.ui \
	parameterpage.cpp parameterview.cpp pref.cpp programinformation.cpp programpage.cpp \
	programspage.cpp queryinputdialog.cpp queryoutputdialog.cpp spikelayout.ui spikepage.cpp \
	tags.cpp unitlistlayout.cpp unitlistpage.cpp unittable.cpp unittableitem.cpp \
	videolayout.ui videopage.cpp xmlreader.cpp xmlwriter.cpp
AM_CXXFLAGS = -pedantic
SUBDIRS = icons
