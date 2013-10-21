#////////////////////////////////////////////////////////////////////////////
#//   Makefile to compile and install process_extractspikes
#//
#//   copyright            : (C) 2007-2011 by Nicolas LEBAS
#//   email                : nicolas.lebas@college-de-france.fr
#//   copyright            : (C) 2008 by Michaël Zugaro
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
EXEC = process_extractspikes

all: $(EXEC) doc

%.o: %.cpp
	$(CC) -c $<

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJ)

MANSRC = $(EXEC).docbook
MANOBJ = $(EXEC).1
MANGZ = $(MANOBJ).gz
XSLTPROC = /usr/bin/xsltproc
LOCAL = \`locate manpages/docbook.xsl|tail -1\`
WEB = http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl
XSL = `a=$(LOCAL) && [ -z "$(LOCAL)" ] && echo $(WEB) || echo $(LOCAL)`
GZIP = gzip -9

doc: $(MANGZ)

$(MANGZ): $(MANSRC)
	$(XSLTPROC) $(XSL) $(MANSRC)
	$(GZIP) $(MANOBJ)

clean:
	@echo "Cleaning $(EXEC) and manual page..."
	- rm -f $(EXEC) *.o $(MANOBJ) $(MANGZ) manpage*

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
	- rm $(BINDIR)/$(EXEC) $(MANDIR)/$(MANGZ)

