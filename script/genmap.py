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

def format_lmp1(name, code, greek, type_table):
    if type_table.has_key(name):
        type = type_table[name]
        force_upright = 1
    elif greek:
        type = 'Ord'
        force_upright = 0
    else:
        type = '-'
        force_upright = 1
    line = '%s\t%s\t%d\t%s' % (code, type, force_upright, name)
    if greek or name == '\\dbar':
        line += '\n%s\t%s\t%d\t%s' % \
              (code, type, 1, name[:1] + 'up' + name[1:])
    return line

def extract_math_blackboard_bold(unicode):
    return extract_math_alphabet(unicode, 'DOUBLE-STRUCK')

def extract_math_script_italic(unicode):
    return extract_math_alphabet(unicode, 'SCRIPT')

def extract_math_script_bold_italic(unicode):
    return extract_math_alphabet(unicode, 'BOLD SCRIPT')

def extract_math_fraktur_regular(unicode):
    return extract_math_alphabet(
        unicode, 'FRAKTUR', 'BLACK-LETTER')

def extract_math_fraktur_bold(unicode):
    return extract_math_alphabet(
        unicode, 'BOLD FRAKTUR', 'BOLD BLACK-LETTER')

def extract_math_sans_serif_regular(unicode):
    return extract_math_alphabet(unicode, 'SANS-SERIF')

def extract_math_sans_serif_italic(unicode):
    return extract_math_alphabet(unicode, 'SANS-SERIF ITALIC')

def extract_math_sans_serif_bold(unicode):
    return extract_math_alphabet(unicode, 'SANS-SERIF BOLD')

def extract_math_sans_serif_bold_italic(unicode):
    return extract_math_alphabet(unicode, 'SANS-SERIF BOLD ITALIC')

def extract_math_monospace_regular(unicode):
    return extract_math_alphabet(unicode, 'MONOSPACE')

def format_math_alpha(name, code):
    int_code = int(code, 16)
    if int_code < 65536:
        line = 'case \'%s\': _glyph = %s; break;' % \
               (maputil.escape_c_form(name),
                maputil.ucs_c_form(code))
    elif name.isupper():
        line = '_glyph = %s + (_code[0] - \'A\');' % \
               maputil.ucs_c_form(int_code -
                                  (ord(name[0]) - ord('A')))
    elif name.islower():
        line = '_glyph = %s + (_code[0] - \'a\');' % \
               maputil.ucs_c_form(int_code -
                                  (ord(name[0]) - ord('a')))
    elif name.isdigit():
        line = '_glyph = %s + (_code[0] - \'0\');' % \
               maputil.ucs_c_form(int_code -
                                  (ord(name[0]) - ord('0')))
    else:
        line = '(unhandled Unicode glyph %s)' % code
    return line

def print_lmp1(table):
    for name, code, greek in table:
        print format_lmp1(name, code, greek, type_table)

def print_math_alpha(table):
    last_line = ''
    for name, code in table:
        line = format_math_alpha(name, code)
        if line != last_line:
            print line
            last_line = line

type_table = maputil.read_plain_tex(maputil.plain_tex_path)
unicode_data_filename = os.path.join(
    os.path.dirname(sys.argv[0]), '..', 'data', 'UnicodeData.txt')
unicode = maputil.load_unicode(unicode_data_filename)

if sys.argv[1] == '--lmp1':
    print_lmp1(maputil.extract_lmp1(unicode))
elif sys.argv[1] == '--blackboard-bold':
    print_math_alpha(maputil.extract_math_blackboard_bold(unicode))
elif sys.argv[1] == '--script-italic':
    print_math_alpha(maputil.extract_math_script_italic(unicode))
elif sys.argv[1] == '--script-bold-italic':
    print_math_alpha(maputil.extract_math_script_bold_italic(unicode))
elif sys.argv[1] == '--fraktur-regular':
    print_math_alpha(maputil.extract_math_fraktur_regular(unicode))
elif sys.argv[1] == '--fraktur-bold':
    print_math_alpha(maputil.extract_math_fraktur_bold(unicode))
elif sys.argv[1] == '--sans-serif-regular':
    print_math_alpha(maputil.extract_math_sans_serif_regular(unicode))
elif sys.argv[1] == '--sans-serif-italic':
    print_math_alpha(maputil.extract_math_sans_serif_italic(unicode))
elif sys.argv[1] == '--sans-serif-bold':
    print_math_alpha(maputil.extract_math_sans_serif_bold(unicode))
elif sys.argv[1] == '--sans-serif-bold-italic':
    print_math_alpha(maputil.extract_math_sans_serif_bold_italic(unicode))
elif sys.argv[1] == '--monospace-regular':
    print_math_alpha(maputil.extract_math_monospace_regular(unicode))
