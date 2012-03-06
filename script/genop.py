#!/usr/bin/env python

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

import os, sys, maputil

def read_large_operator_table(filename_list):
    table = []
    character_line = []
    uncoded_line = []
    for filename in filename_list:
        file = open(filename, 'r')
        try:
            content = file.read().split('\n')
        finally:
            file.close()
        for line in content:
            if line[:1] == '#':
                continue
            if line.find('\t') != -1:
                slot, type, upright, code = line.split('\t')
                if slot != '-':
                    if code[:1] == '\\':
                        table.append([slot, type, upright, code])
                    else:
                        character_line.append(line)
                else:
                    uncoded_line.append(line)
    table.sort(lambda u, v: cmp(u[3], v[3]))
    return table, character_line, uncoded_line

large_operator_table, character_line, uncoded_line = \
                      read_large_operator_table([
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'mt2exa_map.txt')])
text_operator_table = maputil.read_plain_tex_text_operator(
    maputil.plain_tex_path_operator)

table = []
for slot, type, upright, code in large_operator_table:
    table.append([code, [True, slot, None, False]])
    if code.find('intop') != -1:
        table.append([code.replace('intop', 'int'),
                      [True, slot, None, True]])
for let_to in text_operator_table.keys():
    table.append([let_to, text_operator_table[let_to]])
table.sort(lambda u, v: cmp(u, v))

print 'static const unsigned long __noperator = %d;' % len(table)
print 'static const char *__operator_name[__noperator] = {'
for operator, list in table:
    print '    "' + operator.replace('\\', '\\\\') + '",'
print '};'
print
print 'static const wchar_t __operator_code_point[__noperator] = {'
for operator, list in table:
    symbol, code, text, nolimits = list
    if symbol:
        print '    ' + maputil.ucs_c_form(code) + ','
    else:
        print '    ' + maputil.ucs_c_form(0) + ','
print '};'
print
print 'static const char *__operator_control_sequence[__noperator] = {'
for operator, list in table:
    symbol, code, text, nolimits = list
    if symbol:
        print '    "",'
    else:
        print '    "' + text.replace('\\', '\\\\') + '",'
print '};'
print
print 'static const bool __operator_nolimits[__noperator] = {'
for operator, list in table:
    symbol, code, text, nolimits = list
    if nolimits:
        print '    true,'
    else:
        print '    false,'
print '};'
