# mathtext - A TeX/LaTeX compatible rendering library. Copyright (C)
# 2008-2012 Yue Shi Lai <ylai@users.sourceforge.net>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <https://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.23)
project(mathtext CXX)

include(GNUInstallDirs)

find_package(SDL)
find_package(OpenGL)
find_package(PkgConfig)
if(PkgConfig_FOUND)
  pkg_check_modules(FTGL IMPORTED_TARGET ftgl)
  pkg_check_modules(LCMS IMPORTED_TARGET lcms)
endif()

add_subdirectory(src)
