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
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

import os

def config(configuration, search_path = []):
    for path_sdl in [None] + search_path:
        header_sdl = os.path.join('SDL', 'SDL.h')
        if path_sdl != None:
            header_sdl = os.path.join(path_sdl, 'include', header)
        if configuration.CheckCHeader(header_sdl):
            configuration.env.Append(CPPDEFINES = ['HAVE_SDL'])
            configuration.env.SetDefault(
                SDL_LIBS = ['SDL'])
            if configuration.env['ARCH'] == 'x86_64':
                lib_directory = 'lib64'
            else:
                lib_directory = 'lib'
            if path_sdl != None:
                configuration.env.Append(
                    CPPPATH = [os.path.join(path_sdl,
                                            'include')],
                    LIBPATH = [os.path.join(path_sdl,
                                            lib_directory)])
            break
