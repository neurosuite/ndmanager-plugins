#////////////////////////////////////////////////////////////////////////////
#//   Makefile for description files
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

INSTALL_DIR = /usr
DESCRIPTIONS_DIR = $(INSTALL_DIR)/share/ndmanager/descriptions
DESCRIPTIONS = $(shell ls |grep -v makefile)
MAKE = /usr/bin/make --no-print-directory
INSTALL = /usr/bin/install

all:

install:
	@echo "***********************************************************************************"
	@echo "  Description files will be installed in $(DESCRIPTIONS_DIR)"
	@echo "***********************************************************************************"
	@$(INSTALL) -d $(DESCRIPTIONS_DIR)
	@for i in $(DESCRIPTIONS); do echo "Installing $$i..."; ($(INSTALL) -m 644 $$i $(DESCRIPTIONS_DIR)); done


uninstall:
	@for i in $(DESCRIPTIONS); do echo "Uninstalling $$i..."; (rm -f $(DESCRIPTIONS_DIR)/$$i); done
	@rmdir $(DESCRIPTIONS_DIR)

clean: