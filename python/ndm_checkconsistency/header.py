#!/usr/bin/python
# -*- coding: utf-8 -*-

try:
    from PyQt6 import QtCore, QtGui, QtWidgets
except ImportError:
    try:
        from PyQt5 import QtCore, QtGui, QtWidgets
    except ImportError:
        from PyQt4 import QtCore, QtGui
import os,re,fnmatch
