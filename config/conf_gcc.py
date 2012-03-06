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

import os, glob

def config(configuration, capability, debug, ipo):
    # System-wide local include and library path
    configuration.env.Append(CPPPATH = ['/usr/local/include'])
    if configuration.env['ARCH'] == 'x86_64':
        configuration.env.Append(LIBPATH = '/usr/local/lib64')
    else:
        configuration.env.Append(LIBPATH = '/usr/local/lib')
    ret = 1
    configuration.Result(ret)
    return ret

def warn_error(configuration, disable_more = []):
    warn_c = [
        '-Wall',
        '-Wextra',
        '-Wno-unused-parameter',
        '-Wno-enum-compare',
        '-Werror',
        '-fstack-protector']
    warn_fortran = warn_c
    configuration.env.Append(
        CXXFLAGS = warn_c,
        SHCXXFLAGS = warn_c,
        FORTRANFLAGS = warn_fortran,
        SHFORTRANFLAGS = warn_fortran)
