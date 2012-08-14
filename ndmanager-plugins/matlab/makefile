#////////////////////////////////////////////////////////////////////////////
#//   Makefile for the matlab scripts/functions man pages
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


INSTALL_DIR = /usr
MATLAB_DIR = $(INSTALL_DIR)/local/matlab
MAN_BASE = $(INSTALL_DIR)/man
MAN_DIR = $(MAN_BASE)/man1
INSTALL = /usr/bin/install

install:
	@echo "***********************************************************************************"
	@echo "  Matlab scripts will be installed in $(MATLAB_DIR)"
	@echo "***********************************************************************************"
	@$(INSTALL) -d $(MATLAB_DIR)
	@for i in $(SCRIPTS); do echo "Installing $$i..."; ($(INSTALL) $$i $(MATLAB_DIR)); done
	@echo "Installing manual pages for matlab scripts..."
	@$(INSTALL) -d $(MAN_DIR)
	@for i in $(MAN_PAGES); do ($(INSTALL) $$i $(MAN_DIR)); done

uninstall:
	@- for i in $(SCRIPTS); do echo "Uninstalling $$i..."; (rm $(MATLAB_DIR)/$$i); done
	@- rmdir $(MATLAB_DIR)
	@echo "Uninstalling manual pages for matlab scripts..."
	@- for i in $(MAN_PAGES); do (rm $(MAN_DIR)/$$i); done

clean:
	@echo "Cleaning manual pages for matlab scripts..."
	@- for i in $(MAN_PAGES); do (rm $$i); done

