#////////////////////////////////////////////////////////////////////////////
#//   Makefile for ndm_checkconsistency
#//
#//   copyright            : (C) 2011 by Michaël Zugaro
#//   email                : michael.zugaro@college-de-france.fr
#//
#//   This program is free software; you can redistribute it and/or modify
#//   it under the terms of the GNU General Public License as published by
#//   the Free Software Foundation; either version 3 of the License, or
#//   (at your option) any later version.
#//
#////////////////////////////////////////////////////////////////////////////

HDR = header.py
SRC = main.py
RCC = ressources.qrc
RESSOURCES = $(RCC:.qrc=.py)
IMAGES = $(wildcard images/*.png)
EXEC = ndm_checkconsistency
UI = $(wildcard *.ui)
UIC = $(UI:.ui=.py)
INSTALL_DIR = /usr
BIN_DIR = $(INSTALL_DIR)/bin
DESKTOP_FILES_KDE3 = $(wildcard *kde3.desktop)
DESKTOP_FILES_KDE4 = $(wildcard *kde4.desktop)
DESKTOP_DIR_KDE3 = /usr/share/apps/konqueror/servicemenus/
DESKTOP_DIR_KDE4 = /usr/share/kde4/services/ServiceMenus/
INSTALL = /usr/bin/install
PYUIC = /usr/bin/pyuic4
PYRCC = /usr/bin/pyrcc4

MANSRC = $(EXEC).docbook
MANOBJ = $(EXEC).1
MANGZ = $(MANOBJ).gz
XSLTPROC = /usr/bin/xsltproc
LOCAL = $(shell locate manpages/docbook.xsl|tail -1)
WEB = http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl
XSL = $(shell [ -z "$(LOCAL)" ] && echo $(WEB) || echo $(LOCAL))
GZIP = gzip -9
MAN_DIR = $(INSTALL_DIR)/man/man1

all: $(EXEC) doc

doc: $(MANGZ)

$(MANGZ): $(MANSRC)
	@$(XSLTPROC) $(XSL) $(MANSRC)
	@$(GZIP) $(MANOBJ)

%.py: %.ui
	@for i in $<; do $(PYUIC) -o $${i%ui}py $$i; done

$(RESSOURCES): $(RCC) $(IMAGES)
	@$(PYRCC) -o $(RESSOURCES) $(RCC)

$(EXEC): $(HDR) $(UIC) $(RESSOURCES) $(SRC)
	@cat $(HDR) $(UIC) $(RESSOURCES) $(SRC) | grep -v ressources_rc > $(EXEC)
	@chmod a+x $(EXEC)

install:
	@echo "Installing $(EXEC)..."
	@$(INSTALL) -d $(BIN_DIR)
	@$(INSTALL) $(EXEC) $(BIN_DIR)
	@$(INSTALL) -d $(DESKTOP_DIR_KDE3)
	@for i in $(DESKTOP_FILES_KDE3); do ($(INSTALL) $$i $(DESKTOP_DIR_KDE3)); done
	@$(INSTALL) -d $(DESKTOP_DIR_KDE4)
	@for i in $(DESKTOP_FILES_KDE4); do ($(INSTALL) $$i $(DESKTOP_DIR_KDE4)); done
	@$(INSTALL) -d $(MAN_DIR)
	@$(INSTALL) $(MANGZ) $(MAN_DIR)

uninstall:
	@- rm $(BIN_DIR)/$(EXEC)
	@- for i in $(DESKTOP_FILES_KDE3); do (rm $(DESKTOP_DIR_KDE3)/$$i); done
	@- for i in $(DESKTOP_FILES_KDE4); do (rm $(DESKTOP_DIR_KDE4)/$$i); done

clean:
	@- rm $(UIC) $(EXEC) $(RESSOURCES) $(MANGZ)
