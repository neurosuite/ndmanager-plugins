#////////////////////////////////////////////////////////////////////////////
#//   Makefile for python programs
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

MAKE = /usr/bin/make --no-print-directory
PYTHON_DIRS = $(shell ls|grep -v makefile|grep -v test)

all:
	@for i in $(PYTHON_DIRS); do echo "Making $$i..."; (cd $$i; $(MAKE)); done

install:
	@for i in $(PYTHON_DIRS); do (cd $$i; $(MAKE) install); done

uninstall:
	@for i in $(PYTHON_DIRS); do (cd $$i; $(MAKE) uninstall); done

clean:
	@for i in $(PYTHON_DIRS); do (cd $$i; $(MAKE) clean); done
