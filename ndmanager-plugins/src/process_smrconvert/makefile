#////////////////////////////////////////////////////////////////////////////
#//   Makefile to compile and install process_smrconvert
#//
#//   copyright            : (C) 2008-2011 by Michaël Zugaro
#//   email                : michael.zugaro@college-de-france.fr
#//
#//   This program is free software; you can redistribute it and/or modify
#//   it under the terms of the GNU General Public License as published by
#//   the Free Software Foundation; either version 3 of the License, or
#//   (at your option) any later version.
#//
#////////////////////////////////////////////////////////////////////////////

CC = g++ -O3
LDFLAGS = -lstdc++
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = process_smrconvert

all: $(EXEC) doc

process_smrconvert.o: process_smrconvert.cpp sonextractor.h sonfile.h
sonadcchannels.o: sonadcchannels.cpp sonadcchannels.h sonblockheaders.h sonerror.h sonchannel.h sonfile.h
sonadcmarkchannel.o: sonadcmarkchannel.cpp sonadcmarkchannel.h sonfileheader.h sonadcmarkchannel.h sonblockheaders.h sonfile.h sonchannel.h sonerror.h
sonblockheaders.o: sonblockheaders.cpp sonblockheaders.h sonerror.h sonchannelinfo.h sonfile.h
sonchannel.o: sonchannel.cpp sonchannel.h sonchannelinfo.h sonfile.h
sonchannelinfo.o: sonchannelinfo.cpp sonchannelinfo.h sonfile.h
sonerror.o: sonerror.cpp sonerror.h
soneventchannel.o: soneventchannel.cpp soneventchannel.h soneventchannel.h sonblockheaders.h sonfileheader.h sonchannel.h sonerror.h sonfile.h
sonextractor.o: sonextractor.cpp sonextractor.h sonfileheader.h sonchannelinfo.h sonadcchannels.h sonadcmarkchannel.h soneventchannel.h sonerror.h sonfile.h
sonfileheader.o: sonfileheader.cpp sonfileheader.h sonfile.h
sonfile.o: sonfile.cpp sonfile.h

%.o: %.cpp %.h
	$(CC) -c $<

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJ)

MANSRC = $(EXEC).docbook
MANOBJ = $(EXEC).1
MANGZ = $(MANOBJ).gz
XSLTPROC = /usr/bin/xsltproc
LOCAL = $(shell locate manpages/docbook.xsl|tail -1)
WEB = http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl
XSL = $(shell [ -z "$(LOCAL)" ] && echo $(WEB) || echo $(LOCAL))
GZIP = gzip -9

doc: $(MANGZ)

$(MANGZ): $(MANSRC)
	$(XSLTPROC) $(XSL) $(MANSRC)
	$(GZIP) $(MANOBJ)

clean:
	@echo "Cleaning $(EXEC) and manual page..."
	@- rm -f $(EXEC) *.o $(MANOBJ) $(MANGZ)

INSTALLDIR = /usr
BINDIR = $(INSTALLDIR)/bin
MANBASE = $(INSTALLDIR)/man
MANDIR = $(MANBASE)/man1
INSTALL = /usr/bin/install

install:
	@echo "Installing $(EXEC)..."
	@$(INSTALL) -d $(BINDIR)
	@$(INSTALL) $(EXEC) $(BINDIR)
	@echo "Installing manual page for $(EXEC)..."
	@$(INSTALL) -d $(MANDIR)
	@$(INSTALL) $(MANGZ) $(MANDIR)

uninstall:
	@echo "Uninstalling $(EXEC) and manual page..."
	@rm $(BINDIR)/$(EXEC) $(MANDIR)/$(MANGZ)

