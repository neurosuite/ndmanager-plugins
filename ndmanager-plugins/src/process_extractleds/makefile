#////////////////////////////////////////////////////////////////////////////
#//   Makefile to compile and install process_extractleds
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

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.cpp=.o)
EXEC = process_extractleds
FFMPEG = $(shell ls -d ffmpeg*)

all: $(EXEC) doc

$(EXEC): $(OBJ)
	cp $(SRC) $(FFMPEG)/ffplay.c
	cd $(FFMPEG) && ./configure && make ffplay
	cp $(FFMPEG)/ffplay $(EXEC)

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
	@- cd $(FFMPEG) && make clean

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

