#////////////////////////////////////////////////////////////////////////////
#//   Makefile for the ndm man pages
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

DOCBOOK = $(wildcard *.docbook)
MAN_PAGES = $(DOCBOOK:.docbook=.1.gz)
SCRIPTS = $(shell ls|grep -v makefile|grep -v [.]1|grep -v docbook)

all: doc

XSLTPROC = /usr/bin/xsltproc
LOCAL = $(shell locate manpages/docbook.xsl|tail -1)
WEB = http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl
XSL = $(shell [ -z "$(LOCAL)" ] && echo $(WEB) || echo $(LOCAL))
GZIP = gzip -9

doc: $(MAN_PAGES)

$(MAN_PAGES): %.1.gz: %.docbook
	@$(XSLTPROC) $(XSL) $<
	@$(GZIP) `echo $@|sed 's/.gz//'`


INSTALLDIR = /usr
BIN_DIR = $(INSTALLDIR)/bin
MAN_BASE = $(INSTALLDIR)/man
MAN_DIR = $(MAN_BASE)/man1
INSTALL = /usr/bin/install

install:
	@$(INSTALL) -d $(BIN_DIR)
	@for i in $(SCRIPTS); do echo "Installing $$i..."; ($(INSTALL) $$i $(BIN_DIR)); done
	@echo "Installing manual pages for scripts..."
	@$(INSTALL) -d $(MAN_DIR)
	@for i in $(MAN_PAGES); do ($(INSTALL) $$i $(MAN_DIR)); done

uninstall:
	@- for i in $(SCRIPTS); do echo "Uninstalling $$i..."; (rm $(BIN_DIR)/$$i); done
	@echo "Uninstalling manual pages for scripts..."
	@- for i in $(MAN_PAGES); do (rm $(MAN_DIR)/$$i); done

clean:
	@echo "Cleaning manual pages for scripts..."
	@- for i in $(MAN_PAGES); do (rm $$i); done

