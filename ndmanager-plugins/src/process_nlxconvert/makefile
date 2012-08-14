#////////////////////////////////////////////////////////////////////////////
#//  Makefile to compile and install process_ncs2dat
#//
#//  copyright      : (C) 2009-2011 by Michaël Zugaro
#//  email        : michael.zugaro@college-de-france.fr
#//
#//  This program is free software; you can redistribute it and/or modify
#//  it under the terms of the GNU General Public License as published by
#//  the Free Software Foundation; either version 3 of the License, or
#//  (at your option) any later version.
#//
#////////////////////////////////////////////////////////////////////////////

CC = g++ -O3
LDFLAGS = -lstdc++ -lm
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = process_nlxconvert

all: $(EXEC) doc

CUSTOM_TYPES_H = customtypes.h
NLX_ERROR_H = nlxerror.h
PROGRESS_BAR_H = progressbar.h
OUTPUT_FILE_H = outputfile.h $(NLX_ERROR_H)
NCS_FILE_H = ncsfile.h $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
NEV_FILE_H = nevfile.h $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
NVT_FILE_H = nvtfile.h $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
SMI_FILE_H = smifile.h $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
NCS_FILE_COLLECTION_H = ncsfilecollection.h $(NCS_FILE_H) $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
SMI_FILE_COLLECTION_H = smifilecollection.h $(SMI_FILE_H) $(CUSTOM_TYPES_H) $(NLX_ERROR_H)
NLX_EXTRACTOR_H = nlxextractor.h $(CUSTOM_TYPES_H) $(NCS_FILE_COLLECTION_H) $(NVT_FILE_H) $(NEV_FILE_H) $(SMI_FILE_COLLECTION_H) $(OUTPUT_FILE_H)

nlxerror.o: nlxerror.cpp $(NLX_ERROR_H)
outputfile.o: outputfile.cpp $(OUTPUT_FILE_H)
ncsfile.o: ncsfile.cpp $(NCS_FILE_H)
nevfile.o: nevfile.cpp $(NEV_FILE_H)
nvtfile.o: nvtfile.cpp $(NVT_FILE_H)
smifile.o: smifile.cpp $(SMI_FILE_H)
ncsfilecollection.o: ncsfilecollection.cpp $(NCS_FILE_COLLECTION_H)
smifilecollection.o: smifilecollection.cpp $(SMI_FILE_COLLECTION_H)
progressbar.o: progressbar.cpp $(PROGRESS_BAR_H) $(CUSTOM_TYPES_H)
process_nlxconvert.o: process_nlxconvert.cpp $(NLX_EXTRACTOR_H)
nlxextractor.o: nlxextractor.cpp $(NLX_EXTRACTOR_H) $(NLX_ERROR_H) $(PROGRESS_BAR_H)

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

