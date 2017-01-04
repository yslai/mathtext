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
    for lcms_version in '2', '':
        for path_lcms in [None] + search_path:
            header_lcms = os.path.join('lcms' + lcms_version + '.h')
            if path_lcms != None:
                header_lcms = os.path.join(
                    path_lcms, 'include', header)
            if configuration.CheckCHeader([header_lcms]):
                configuration.env.Append(CPPDEFINES = [
                    'HAVE_LCMS' + lcms_version])
                configuration.env.SetDefault(LCMS_LIBS = [
                    'lcms' + lcms_version])
                if configuration.env['ARCH'] == 'x86_64':
                    lib_directory = 'lib64'
                else:
                    lib_directory = 'lib'
                if path_lcms != None:
                    configuration.env.Append(
                        CPPPATH = [
                            os.path.join(path_lcms, 'include')],
                        LIBPATH = [
                            os.path.join(path_lcms, lib_directory)])
                break
