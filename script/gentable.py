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

filename_list = [
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'lmp1_map.txt'),
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'lmp2_map.txt'),
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'mt2sya_map.txt'),
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'lucida_map.txt'),
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'composite_map.txt')]

filename_list_alias = [
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data', 'table',
                 'alias_map.txt')]

def read_table(filename_list):
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

def read_table_alias(filename_list):
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
                def_to, def_from = line.split('\t')
                table.append([def_to, def_from])
    table.sort(lambda u, v: cmp(u[0], v[0]))
    return table

def print_c_form(table):
    count = 0
    for slot, type, upright, code in table:
        count += 1
    print 'static const unsigned long nglyph = %d;' % count
    print 'static const char *glyph_control_sequence[nglyph] = {'
    for slot, type, upright, code in table:
        print '    "' + maputil.escape_c_form(code) + '",'
    print '};'
    print
    print 'static const unsigned int glyph_type[nglyph] = {'
    for slot, type, upright, code in table:
        if type == 'Alpha':
            type = 'Ord'
        if type != '-':
            print '    atom_t::TYPE_' + type.upper() + ','
        else:
            print '    atom_t::TYPE_ORD,'
    print '};'
    print
    print 'static const bool glyph_upright[nglyph] = {'
    for slot, type, upright, code in table:
        if type != '-':
            if upright != '0':
                print '    true,'
            else:
                print '    false,'
        else:
            print '    true,'
    print '};'
    print
    print 'static const wchar_t glyph_code_point[nglyph] = {'
    for slot, type, upright, code in table:
        print '    ' + maputil.ucs_c_form(slot) + ','
    print '};'

table, character_line, uncoded_line = read_table(filename_list)
table_alias = read_table_alias(filename_list_alias)
for def_to, def_from in table_alias:
    found = False
    for slot, type, upright, code in table:
        if code == def_to:
            print >> sys.stderr, 'target glyph `' + def_to + \
                  '\' defined (`' + def_from + '\' intented?)'
            break
        elif code == def_from:
            table.append([slot, type, upright, def_to])
            found = True
            break
    if found == False:
        print >> sys.stderr, 'source glyph `' + def_to + '\' undefined'
table.sort(lambda u, v: cmp(u[3], v[3]))

if sys.argv[1] == '--table':
    print_c_form(table)
elif sys.argv[1] == '--table-typeless':
    for slot, type, upright, code in table:
        if type == '-':
            print slot + '\t' + type + '\t' + upright + '\t' + code
elif sys.argv[1] == '--character':
    print '\n'.join(character_line)
elif sys.argv[1] == '--uncoded':
    print '\n'.join(uncoded_line)
