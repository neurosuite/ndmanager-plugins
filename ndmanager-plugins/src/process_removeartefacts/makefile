#////////////////////////////////////////////////////////////////////////////
#//   Makefile to compile and install process_removeartefacts
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
HDR = $(wildcard *.h)
OBJ = $(SRC:.cpp=.o)
EXEC = process_removeartefacts

all: $(EXEC) doc

clean:
	@echo "Cleaning $(EXEC) and manual page..."
	@- rm -f $(EXEC) *.o $(MANOBJ) $(MANGZ)

%.o: %.cpp
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

