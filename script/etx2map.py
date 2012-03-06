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

import sys, maputil

glyph_table = maputil.read_glyph_list(maputil.glyph_list_path)
type_table = maputil.read_plain_tex(maputil.plain_tex_path)
table = []
for code, slot in maputil.read_etx(sys.argv[1]):
    if type_table.has_key(code):
        type = type_table[code]
    else:
        type = ''
    if glyph_table.has_key(slot):
        slot = ','.join(glyph_table[slot])
    else:
        slot = ''
    table.append([slot, type, 1, code])
table.sort(lambda u, v:
           cmp(int('0' + u[0].split(',')[0], 16),
               int('0' + v[0].split(',')[0], 16)))

for slot, type, upright, code in table:
    if slot == '':
        slot = '-'
    if type == '':
        type = '-'
    print >> sys.stdout, "%s\t%s\t%d\t%s" % \
          (slot, type, upright, code)

# grep '^\\def\\[a-z]*{\\mathop{'
