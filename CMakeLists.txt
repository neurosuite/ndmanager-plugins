# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)
project(ndmanager-plugins
    VERSION 3.0.0
    DESCRIPTION "Processing tools and scripts for NDManager"
    HOMEPAGE_URL "https://neurosuite.github.io"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WITH_PYTHON_TOOLS "Build ndm_prepare and ndm_checkconsistency (needs PyQt6's pyuic6)" ON)
option(WITH_MANPAGES "Build manual pages from the DocBook sources (needs xsltproc)" ON)

include(GNUInstallDirs)
include(cmake/Manpages.cmake)

add_subdirectory(src)
add_subdirectory(scripts)
add_subdirectory(descriptions)
add_subdirectory(matlab)
if(WITH_PYTHON_TOOLS)
    add_subdirectory(python)
endif()

include(cmake/CPackConfig.cmake)
