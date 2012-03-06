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
    configuration.Message('Checking for Intel C++/Fortran ' +
                          'Compiler... ')
    # The following options are roughtly equivalent to what is being
    # used by Intel and AMD for the SPEC CPU 2006 benchmark.
    code_generation = []
    if not debug:
        if 'HAVE_SSSE3' in capability:
            code_generation = ['-xSSSE3']
        elif 'HAVE_SSE3' in capability:
            code_generation = ['-xSSE3']
        elif 'HAVE_SSE2' in capability:
            code_generation = ['-xSSE2']
        elif 'HAVE_SSE' in capability and \
                 configuration.env['ARCH'] != 'x86_64':
            code_generation = ['-mia32', '-axSSE2']
    # Common compiler arguments and paths
    if debug:
        optimize = ['-g', '-O0', ['-fp-model', 'source']] + \
                   code_generation
    else:
        optimize = ['-O3', '-no-prec-div'] + code_generation
    if ipo:
        optimize.append('-ipo')
    configuration.env.Append(CPPDEFINES = capability)
    configuration.env['LIBRARY_SOURCE'] = []
    if os.uname()[4] == 'x86_64' and \
           configuration.env['ARCH'] != 'x86_64':
        if os.path.exists(os.path.join('/opt', 'intel', 'composerxe')):
            icccom, icpccom, ifortcom, xiarcom = map(
                lambda c: glob.glob(os.path.join(
                '/opt', 'intel', 'composerxe-*.*', 'bin', 'ia32',
                c))[0],
                ('icc', 'icpc', 'ifort', 'xiar'))
        elif os.path.exists(os.path.join('/opt', 'intel', 'Compiler')):
            icccom, icpccom, ifortcom, xiarcom = map(
                lambda c: glob.glob(os.path.join(
                '/opt', 'intel', 'Compiler', '*', '*', 'bin', 'ia32',
                c))[0],
                ('icc', 'icpc', 'ifort', 'xiar'))
        else:
            icccom = glob.glob(os.path.join(
                '/opt', 'intel', 'cc', '*', 'bin', 'icc'))[0]
            icpccom = glob.glob(os.path.join(
                '/opt', 'intel', 'cc', '*', 'bin', 'icpc'))[0]
            ifortcom = glob.glob(os.path.join(
                '/opt', 'intel', 'fc', '*', 'bin', 'ifort'))[0]
            xiarcom = glob.glob(os.path.join(
                '/opt', 'intel', 'cc', '*', 'bin', 'xiar'))[0]
    else:
        icccom = 'icc'
        icpccom = 'icpc'
        ifortcom = 'ifort'
        xiarcom = 'xiar'
    configuration.env.Replace(
        CC = icccom,
        CXX = icpccom,
        SHCC = icccom,
        SHCXX = icpccom)
    configuration.env.Replace(AR = xiarcom)
    # C++ compiler
    configuration.env.Append(
        CCFLAGS = optimize,
        SHCCFLAGS = optimize)
    # Enable quadruple precision floating point number for C/C++
    cpp_quadruple_precision = ['-Qoption,cpp,--extended_float_type']
    configuration.env.Append(
        CCFLAGS = cpp_quadruple_precision,
        SHCCFLAGS = cpp_quadruple_precision)
    std_cplusplus0x = ['-std=c++0x']
    configuration.env.Append(
        CXXFLAGS = std_cplusplus0x,
        SHCXXFLAGS = std_cplusplus0x)
    # Fortran compiler
    configuration.env.Replace(
        FORTRAN = ifortcom,
        SHFORTRAN = ifortcom)
    configuration.env.Append(
        FORTRANFLAGS = optimize,
        SHFORTRANFLAGS = optimize)
    # Suppress Fortran interface generation (which appears to be
    # activated by default in Intel Fortran Compiler 11.1, possibly a
    # bug)
    generate_interface = ['-nogen-interfaces']
    configuration.env.Append(
        FORTRANFLAGS = generate_interface,
        SHFORTRANFLAGS = generate_interface)
    # Use wide statement field in fixed-form source
    statement_field_width = ['-extend-source', '132']
    configuration.env.Append(
        FORTRANFLAGS = statement_field_width,
        SHFORTRANFLAGS = statement_field_width)
    # Use 64 bit INTEGER and REAL for Fortran
    variable_size = ['-integer-size', '64', '-real-size', '64']
    configuration.env.Append(
        FORTRANFLAGS = variable_size,
        SHFORTRANFLAGS = variable_size)
    # System-wide local library path
    if configuration.env['ARCH'] == 'x86_64':
        configuration.env.Append(LIBPATH = '/usr/local/lib64')
    else:
        configuration.env.Append(LIBPATH = '/usr/local/lib')
    ret = 1
    configuration.Result(ret)
    return ret

def warn_error(configuration, disable_more = []):
    warn_disable = [
        383,	# value copied to temporary, reference to ...
        810,	# conversion from ... to ... may lose significant bits
        981,	# operands are evaluated in unspecified order
        2259,	# non-pointer conversion from ... to ... may lose ...
        ] + disable_more
    warn_c = [
        '-w2',
        '-wd' + ','.join(['%d' % code for code in warn_disable]),
        '-check-uninit',
        '-ftrapuv']
    warn_fortran = [
        '-warn', 'all',
        '-warn', 'nodeclarations', '-warn', 'nounused']
    configuration.env.Append(
        CXXFLAGS = warn_c,
        SHCXXFLAGS = warn_c,
        FORTRANFLAGS = warn_fortran,
        SHFORTRANFLAGS = warn_fortran)
