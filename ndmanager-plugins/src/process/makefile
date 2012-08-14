#////////////////////////////////////////////////////////////////////////////
#//   Makefile for the general manpage
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

EXEC = process

all: doc

clean:
	@echo "Cleaning manual page for programs..."
	@- rm -f $(EXEC) *.o $(MANOBJ) $(MANGZ)

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
MANBASE = $(INSTALLDIR)/man
MANDIR = $(MANBASE)/man1
INSTALL = /usr/bin/install

install:
	@echo "Installing manual page for programs..."
	@$(INSTALL) -d $(MANDIR)
	@$(INSTALL) $(MANGZ) $(MANDIR)

uninstall:
	@echo "Uninstalling manual page for programs..."
	@rm $(BINDIR)/$(EXEC) $(MANDIR)/$(MANGZ)

