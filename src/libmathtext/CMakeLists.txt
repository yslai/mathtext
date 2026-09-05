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

add_library(mathtext
  fontembed.cc
  fontembedpdf.cc
  fontembedps.cc
  geometry.cc
  mathrender.cc
  mathrenderstyle.cc
  mathrendertoken.cc
  mathtext.cc
  mathtextencode.cc
  mathtextparse.cc
  mathtextview.cc
)
target_include_directories(mathtext PUBLIC .)
set_target_properties(mathtext PROPERTIES
  SOVERSION 0
  POSITION_INDEPENDENT_CODE ON
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wbidi-chars=none GCC_HAS_BIDI_CHARS_FLAG)
if(GCC_HAS_BIDI_CHARS_FLAG)
  set_source_files_properties(fontembed.cc COMPILE_FLAGS "-Wbidi-chars=none")
endif()

if (CMAKE_CXX_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
  target_compile_definitions(mathtext PRIVATE IS_LITTLE_ENDIAN=1)
endif()

target_sources(
  mathtext
  PUBLIC
    FILE_SET headers
    TYPE HEADERS
    FILES
      mathtext/fontembed.h
      mathtext/geometry.h
      mathtext/mathrender.h
      mathtext/mathtext.h
)

install(TARGETS mathtext FILE_SET headers)
