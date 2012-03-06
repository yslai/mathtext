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

import os, re, subprocess

def conftest_file(configuration, index, extension):
    return configuration.env.File(os.path.join(
        '$CONFIGUREDIR',
        'conftest_fortran_%d%s' % (index, extension)))

def config_fortran_link(configuration):
    file_target = conftest_file(configuration, 0, '')
    file_source = conftest_file(configuration, 0, '.f')
    filename_target = str(file_target)
    filename_source = str(file_source)
    file = open(filename_source, 'w')
    try:
        file.write('      PROGRAM MAIN\n      END\n')
    finally:
        file.close()
    command = configuration.env.subst(
        '$FORTRAN -v -o $TARGET $SOURCES',
        target = file_target, source = file_source)
    child = subprocess.Popen(
        command, shell = True,
        stdin = subprocess.PIPE, stdout = subprocess.PIPE,
        stderr = subprocess.PIPE, close_fds = True)
    child.stdout.read()
    content = re.compile('[ \t\n]+').split(child.stderr.read())
    for f in filename_source, filename_target:
        if os.path.exists(f):
            os.remove(f)
    libs = []
    libpath = []
    rpath = []
    for e in content:
        if e[:2] == '-l':
            libs.append(e[2:])
        if e[:2] == '-L':
            libpath.append(e[2:])
        if e[:2] == '-R':
            rpath.append(e[2:])
    if configuration.env['FORTRAN'][-5:] == 'ifort' and \
           'svml' not in libs:
        configuration.env.SetDefault(
            FORTRANLIBS = libs + ['svml'])
    else:
        configuration.env.SetDefault(FORTRANLIBS = libs)
    configuration.env.SetDefault(FORTRANLIBPATH = libpath)
    configuration.env.SetDefault(FORTRANRPATH = rpath)

def config_fortran_mangling(configuration):
    file_target = (conftest_file(configuration, 1, '.o'),
                   conftest_file(configuration, 2, ''))
    file_source = (conftest_file(configuration, 1, '.f'),
                   conftest_file(configuration, 2, '.c'))
    filename_target = map(str, file_target)
    filename_source = map(str, file_source)
    file = open(filename_source[0], 'w')
    try:
        file.write('      SUBROUTINE FOOBAR()\n      RETURN\n' +
                   '      END\n      SUBROUTINE FOO_BAR()\n' +
                   '      RETURN\n      END\n')
    finally:
        file.close()
    command = configuration.env.subst(
        '$FORTRAN -o $TARGET -c $SOURCES',
        target = file_target[0], source = file_source[0])
    child = subprocess.Popen(
        command, shell = True,
        stdin = subprocess.PIPE, stdout = subprocess.PIPE,
        stderr = subprocess.PIPE, close_fds = True)
    child.stdout.read()
    child.stderr.read()
    if not os.path.exists(filename_target[0]):
        return
    command = configuration.env.subst(
        '$CC -o $TARGET $SOURCES',
        target = file_target[1],
        source = [file_target[0], file_source[1]])
    test_prefix = None
    for test_case in 'foobar', 'FOOBAR':
        for test_underscore in '', '_':
            file = open(filename_source[1], 'w')
            try:
                file.write('extern int ' + test_case +
                           test_underscore + '(void);\nint main(' + \
                           'void)\n{\n\treturn ' + test_case +
                           test_underscore + '();\n}\n')
            finally:
                file.close()
            child = subprocess.Popen(
                command, shell = True,
                stdin = subprocess.PIPE, stdout = subprocess.PIPE,
                    stderr = subprocess.PIPE, close_fds = True)
            child.stdout.read()
            child.stderr.read()
            if os.path.exists(filename_target[1]):
                test_prefix = test_case[:3] + '_' + test_case[3:] + \
                              test_underscore
                uppercase = test_case == 'FOOBAR'
                underscore = [test_underscore == '_', None]
                break
        if test_prefix != None:
            break
    if test_prefix == None:
        return
    for test_underscore in '', '_':
        file = open(filename_source[1], 'w')
        try:
            file.write('extern int ' + test_prefix +
                       test_underscore + '(void);\nint main(' + \
                       'void)\n{\n\treturn ' + test_prefix +
                       test_underscore + '();\n}\n')
        finally:
            file.close()
        child = subprocess.Popen(
            command, shell = True,
            stdin = subprocess.PIPE, stdout = subprocess.PIPE,
            stderr = subprocess.PIPE, close_fds = True)
        child.stdout.read()
        child.stderr.read()
        if os.path.exists(filename_target[1]):
            underscore[1] = test_underscore == '_'
            break
    if underscore[1] == None:
        return
    for f in filename_source + filename_target:
        if os.path.exists(f):
            os.remove(f)
    if uppercase:
        name_case = 'NAME'
    else:
        name_case = 'name'
    if underscore[0]:
        suffix = [' ## _', ' ## _']
    else:
        suffix = ['', '']
    if underscore[1]:
        if suffix[1] == '':
            suffix[1] += ' ## '
        suffix[1] += '_'
    cppdefines = [('F77_FUNC(name, NAME)', name_case + suffix[0]),
                  ('F77_FUNC_(name, NAME)', name_case + suffix[1])]
    configuration.env.Append(CPPDEFINES = cppdefines)

def config(configuration):
    configuration.Message('Checking for the Fortran compiler ' +
                          'mangling/linkage... ')
    config_fortran_link(configuration)
    config_fortran_mangling(configuration)
    ret = 1
    configuration.Result(ret)
    return ret
