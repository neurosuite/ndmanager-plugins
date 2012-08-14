#////////////////////////////////////////////////////////////////////////////
#//   Makefile for C/C++ utilities
#//
#//   copyright            : (C) 2004 by Lynn Hazan
#//   email                : lynn.hazan@myrealbox.com
#//   copyright            : (C) 2008-2011 by Michaël Zugaro
#//   email                : michael.zugaro@college-de-france.fr
#//
#//   This program is free software; you can redistribute it and/or modify
#//   it under the terms of the GNU General Public License as published by
#//   the Free Software Foundation; either version 3 of the License, or
#//   (at your option) any later version.
#//
#////////////////////////////////////////////////////////////////////////////

INSTALL = /usr/bin/install -c
MAKE = /usr/bin/make --no-print-directory

SRC_DIRS = $(shell ls|grep -v makefile)

all:
	@for i in $(SRC_DIRS); do echo "Making $$i..."; (cd $$i; $(MAKE)); done

install:
	@for i in $(SRC_DIRS); do (cd $$i; $(MAKE) install); done

uninstall:
	@for i in $(SRC_DIRS); do (cd $$i; $(MAKE) uninstall); done

clean:
	@for i in $(SRC_DIRS); do (cd $$i; $(MAKE) clean); done
