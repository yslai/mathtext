# -*- mode: python; -*-

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

import os, sys, glob

sys.path.append(str(Dir(os.path.join('#', 'config'))))

import cpu, conf_gcc, conf_intel_compiler, conf_fortran, conf_sdl, \
    conf_opengl, conf_ftgl, conf_lcms

# -------------------------------------------------------------------

# SCons performance optimization
DefaultEnvironment(tools = [])

env = Environment()

# SCons performance optimization
env.Decider('MD5-timestamp')
env.SetOption('max_drift', 15 * 60)
env.SetOption('implicit_cache', 1)
env.CacheDir(str(env.Dir(os.path.join('#', 'cache'))))

# Import environment variables
for key in ['HOME', 'PATH', 'LD_LIBRARY_PATH', 'ROOTSYS',
            'OFFLINE_MAIN']:
    if key in os.environ.keys():
        env.Append(ENV = {key : os.environ[key]})

env.Replace(ARCH = ARGUMENTS.get('arch', os.uname()[4]))

conf = Configure(env,
                 {'CheckGCC': conf_gcc.config,
                  'CheckIntel': conf_intel_compiler.config,
                  'CheckFortran': conf_fortran.config})

debugging = ARGUMENTS.get('debug', '0') == '1'
ipo = ARGUMENTS.get('ipo', '0') == '1'

cpu_capability = []
instr_default = ''
if ARGUMENTS.get('instr', instr_default) == 'sse':
    cpu_capability += ['HAVE_SSE']
elif ARGUMENTS.get('instr', instr_default) == 'sse2':
    cpu_capability += ['HAVE_SSE', 'HAVE_SSE2']
elif ARGUMENTS.get('instr', instr_default) == 'sse3':
    cpu_capability += ['HAVE_SSE', 'HAVE_SSE2', 'HAVE_SSE3']
elif ARGUMENTS.get('instr', instr_default) == 'ssse3':
    cpu_capability += ['HAVE_SSE', 'HAVE_SSE2', 'HAVE_SSE3',
                       'HAVE_SSSE3']
else:
    # If not specified, try to autodetect the instruction set
    # capabilities from the current CPU
    cpu_capability += cpu.capability()

# Compiler support
if ARGUMENTS.get('intel_compiler', '0') == '1':
    # Intel C++/Fortran Compilers 9.x/10.x
    conf.CheckIntel(cpu_capability, debugging, ipo)
    conf_intel_compiler.warn_error(conf)
else:
    conf.CheckGCC(cpu_capability, debugging, ipo)
    conf_gcc.warn_error(conf)
conf.CheckFortran()

# SDL + OpenGL + FTGL
if ARGUMENTS.get('sdl', '1') == '1':
    conf_sdl.config(conf)
if ARGUMENTS.get('opengl', '1') == '1':
    conf_opengl.config(conf)
if ARGUMENTS.get('ftgl', '1') == '1':
    conf_ftgl.config(conf)
if ARGUMENTS.get('lcms', '1') == '1':
    conf_lcms.config(conf)

env = conf.Finish()

Export('env')

SConscript('src/SConscript')
env.Clean('.', os.path.join('#', 'build'))
for pattern in 'fort.*', 'config.log', '*~':
    env.Clean('.', env.Glob(os.path.join('#', pattern)))

other_subdirectory = ['config', 'script', 'tool']
for directory in other_subdirectory:
    env.Clean('.', env.Glob(os.path.join('#', directory, '*.pyc')))
    env.Clean('.', env.Glob(os.path.join('#', directory, '*~')))
    env.Clean('.', env.Glob(os.path.join('#', directory, '*', '*~')))
build_subdirectory = ['libgluon', 'libmathtext', 'src']
other_subdirectory = ['build', 'script']
