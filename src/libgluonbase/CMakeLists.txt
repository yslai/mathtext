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

add_library(gluonbase
  color.cc
  geometry.cc
  screen.cc
  surface.cc
  surfaceopengl.cc
  surfacepostscript.cc
)
target_include_directories(gluonbase PUBLIC .)
set_target_properties(gluonbase PROPERTIES
  SOVERSION 0
  POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(gluonbase mathtext)
if(SDL_FOUND)
  target_compile_definitions(gluonbase PUBLIC -DHAVE_SDL=1)
  target_link_libraries(gluonbase SDL::SDL)
endif()
if(OpenGL_FOUND)
  target_compile_definitions(gluonbase PUBLIC -DHAVE_OPENGL=1)
  target_link_libraries(gluonbase OpenGL::OpenGL)
endif()
if(FTGL_FOUND)
  target_compile_definitions(gluonbase PUBLIC -DHAVE_FTGL=1)
  target_link_libraries(gluonbase PkgConfig::FTGL)
endif()
if(LCMS_FOUND)
  target_compile_definitions(gluonbase PUBLIC -DHAVE_LCMS=1)
  target_link_libraries(gluonbase PkgConfig::LCMS)
endif()
