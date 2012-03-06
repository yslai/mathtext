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

import os, sys, re

texmf_path = os.path.join('/usr', 'share', 'texmf')
lcdf_typetools_path = os.path.join('/usr', 'local', 'lcdf-typetools')
plain_tex_path = [
    os.path.join(texmf_path, 'tex', 'latex', 'base', 'fontmath.ltx'),
    os.path.join(texmf_path, 'tex', 'latex', 'amsfonts',
                 'amsfonts.sty'),
    os.path.join(texmf_path, 'tex', 'latex', 'amsfonts',
                 'amssymb.sty'),
    os.path.join('/usr', 'local', 'share', 'texmf', 'tex', 'latex',
                 'mtpro2', 'mtpro2.sty')]
plain_tex_path_operator = [
    os.path.join(texmf_path, 'tex', 'latex', 'base', 'latex.ltx')]

if False:
    plain_tex_path = [
        os.path.join(texmf_path, 'tex', 'plain', 'base',
                     'plain.tex'),
        os.path.join(texmf_path, 'tex', 'plain', 'amsfonts',
                     'amssym.tex')] + plain_tex_path
    plain_tex_path_operator = [
        os.path.join(texmf_path, 'tex', 'plain', 'base',
                     'plain.tex')] + plain_tex_path_operator

glyph_list_path = [
    os.path.join(lcdf_typetools_path, 'share', 'lcdf-typetools',
                 'glyphlist.txt'),
    os.path.join(os.path.dirname(sys.argv[0]), '..', 'data',
                 'source', 'ucs', 'texglyphlist.txt')]

# Knuth (1986), p. 154, pp. 344f.

def tex_strip(line):
    line = re.sub('^[ \t]*', '', line)
    line = re.sub('[ \t]*%.*', '', line)
    line = re.sub('[ \t\r\n]*$', '', line)
    return line

def read_plain_tex(filename_list):
    # Knuth (1986), pp. 154f.
    math_type = ['Ord', 'Op', 'Bin', 'Rel', 'Open', 'Close', 'Punct',
                 'Alpha', 'Active']
    table = {}
    for filename in filename_list:
        print >> sys.stderr, os.path.basename(sys.argv[0]) + \
                          ': loading `' + filename + '\':'
        file = open(filename, 'r')
        try:
            content = file.read()
            content = re.sub(' +\\\\mathchardef', '\n\\mathchardef',
                             content)
            content = re.sub('(\\DeclareMathDelimiter{[^}]*}) *(?:|%.*)\n *', '\\g<1>', content)
            content = content.split('\n')
        finally:
            file.close()
        for line in content:
            line = tex_strip(line)
            line = re.sub('(?<=.) *\\\\let.*', '', line)
            line = re.sub('(?<=.) *\\\\def.*', '', line)
            name = None
            if line[:9] == '\\mathcode' and \
                   line[:13] != '\\mathcode`\\^^' and \
                   line.find('\\string') == -1:
                name = line[11:12]
                if name in ('_', '{', '}'):
                    name = '\\' + name
                type = math_type[int(line[14:15])]
            elif line[:12] == '\\mathchardef' and \
                     line[:14] != '\\mathchardef\\@':
                if line[-4:-3] == '"':
                    name = line[12:-5]
                    type = math_type[0]
                else:
                    name = line[12:-6]
                    type = math_type[int(line[-4:-3])]
            elif line[:10] == '\\newsymbol':
                name = line[10:-5]
                type = math_type[int(line[-4:-3])]
            elif line[:4] == '\\def' and \
                     line.find('{\\delimiter"') != -1:
                if line[-9:-8] == '"':
                    name = line[4:-20]
                    type = math_type[0]
                else:
                    name = line[4:-21]
                    type = math_type[int(line[-9:-8])]
            elif line[:19] == '\\DeclareMathSymbol{' and \
                     line[-1:] == '}':
                argument = re.sub(
                    '} *{', '}{', line[19:-1]).split('}{')
                name = argument[0]
                type = argument[1][5:].capitalize()
            elif line[:22] == '\\DeclareMathDelimiter{' and \
                     line[-1:] == '}':
                argument = re.sub(
                    '} *{', '}{', line[22:-1]).split('}{')
                name = argument[0]
                type = argument[1][5:].capitalize()
            if name != None:
                if table.has_key(name) and table[name] != type and \
                    not (table[name] == 'Ord' and type == 'Alpha'):
                    print >> sys.stderr, \
                          os.path.basename(sys.argv[0]) + \
                          ': warning: type of `' + name + \
                          '\' is redefined from ' + table[name] + \
                          ' to ' + type
                table[name] = type
    return table

def read_plain_tex_alias(filename_list):
    table = {}
    for filename in filename_list:
        print >> sys.stderr, os.path.basename(sys.argv[0]) + \
                          ': loading `' + filename + '\':'
        file = open(filename, 'r')
        try:
            content = file.read()
            content = content.split('\n')
        finally:
            file.close()
        for line in content:
            line = tex_strip(line)
            let_to, let_from = (None, None)
            if line.find('@') == -1:
                if line[:5] == '\\let\\' and line.find('=\\') != -1:
                    let_to, let_from = line.split('=')
                    let_to = let_to[4:]
                elif line[:12] == '\\global\\let\\':
                    dummy0, dummy1, dummy2, let_to, let_from = \
                            line.split('\\')
                    let_to = '\\' + let_to
                    let_from = '\\' + let_from
            if let_from == '\\undefined':
                let_from = None
            if let_to != None and let_from != None:
                if table.has_key(let_to):
                    if table[let_to] != let_from:
                        print >> sys.stderr, \
                              os.path.basename(sys.argv[0]) + \
                              ': warning: alias `' + let_to + \
                              '\' is redefined from ' + \
                              table[let_to] + ' to ' + let_from
                table[let_to] = let_from
    return table

def read_plain_tex_text_operator(filename_list):
    table = {}
    for filename in filename_list:
        print >> sys.stderr, os.path.basename(sys.argv[0]) + \
                          ': loading `' + filename + '\':'
        file = open(filename, 'r')
        try:
            content = file.read()
            content = content.split('\n')
        finally:
            file.close()
        for line in content:
            line = tex_strip(line)
            def_to, def_from, nolimits = (None, None, False)
            if line.find('{\\mathop{\\rm ') != -1:
                def_to, def_from = line.split('{\\mathop{\\rm ')
                if def_from.find('\\nolimits') != -1:
                    nolimits = True
                def_to = def_to[4:]
                def_from = re.sub('}.*', '', def_from)
            elif line.find('{\\mathop{\\operator@font ') != -1:
                def_to, def_from = line.split('{\\mathop{\\operator@font ')
                if def_from.find('\\nolimits') != -1:
                    nolimits = True
                def_to = def_to[4:]
                def_from = re.sub('}.*', '', def_from)
            if def_to != None and def_from != None:
                table[def_to] = [False, 0, def_from, nolimits]
    return table

def read_etx(filename):
    file = open(filename, 'r')
    try:
        content = file.read().split('\n')
    finally:
        file.close()
    in_comment = False;
    table = []
    for line in content:
        line = tex_strip(line)
        if line[:9] == '\\setslot{' and line[-1:] == '}':
            slot = line[9:-1]
        elif line[:9] == '\\comment{':
            in_comment = True
        if line == '\\endsetslot':
            # If there is a math environment, strip everything outside
            # off the comment
            code = re.sub('^[^$]*\$', '', comment)
            code = re.sub('\$.*', '', code)
            # If there is a quote, strip everything outside off the
            # comment
            code = re.sub('^[^`]*`', '', code)
            code = re.sub('(?<!\\\\)\'.*', '', code)
            # Strip off content for e.g. \hat{a}
            code = re.sub('(\\\\[a-z]*){[ax]}', '\\g<1>', code)
            # Strip off the command for e.g. \mathnormal{...}
            code = re.sub('\\\\[a-z]*{([^}]*)}', '\\g<1>', code)
            # Strip off content for e.g. \"a
            code = re.sub('(\\\\[^ax]) *[ax](?![a-z])', '\\g<1>',
                          code)
            # Remaining cases
            if code == '\\not=':
                code = '\\not'
            elif code == '\\t oo':
                code = '\\t'
            table.append([code, slot])
            slot = ''
        elif in_comment:
            if line[:9] == '\\comment{':
                if line[-1] == '}':
                    comment = line[9:-1]
                else:
                    comment = line[9:]
            elif len(line) >= 1 and line[-1] == '}':
                comment += ' ' + line[:-1]
                in_comment = False
            else:
                comment += ' ' + line
    return table

def read_glyph_list(filename_list):
    table = {}
    for filename in filename_list:
        file = open(filename, 'r')
        try:
            content = file.read()
            content = re.sub(' +\\\\mathchardef', '\n\\mathchardef',
                             content)
            content = content.split('\n')
        finally:
            file.close()
        for line in content:
            if line == '' or line[:1] == '#':
                continue
            name, code = line.split(';')
            if code.find(',') == -1:
                code = [code]
            else:
                code = code.split(',')
            if not table.has_key(name):
                table[name] = code
    return table

def load_unicode(filename):
    unicode_data_file = open(filename, 'r')
    try:
        unicode = filter(lambda l: len(l) == 15,
                         map(lambda e: e.split(';'),
                             unicode_data_file.read().split('\n')))
    finally:
        unicode_data_file.close()
    return unicode

def extract_lmp1(unicode):
    table = []
    for entry in unicode:
        code, name, general_category, canonical_combining_class, \
              bidi_class, decomposition_type, numeric_type_decimal, \
              numeric_type_digit, numeric_type_numeric, \
              bidi_mirrored, unicode_1_name, iso_comment, \
              simple_uppercase_mapping, simple_lowercase_mapping, \
              simple_titlecase_mapping = entry
        greek = False
        if name[:6] == 'GREEK ' and name.find(' WITH ') == -1:
            if name == 'GREEK KAI SYMBOL' or \
                   name == 'GREEK LETTER YOT':
                None
            elif name == 'GREEK SMALL LETTER EPSILON':
                name = 'GREEK SMALL LETTER VAREPSILON'
            elif name == 'GREEK LUNATE EPSILON SYMBOL':
                name = 'GREEK SMALL LETTER EPSILON'
            elif name == 'GREEK SMALL LETTER FINAL SIGMA':
                name = 'GREEK SMALL LETTER VARSIGMA'
            elif name == 'GREEK SMALL LETTER PHI':
                name = 'GREEK SMALL LETTER VARPHI'
            elif name[-7:] == ' SYMBOL' and name[6:-7].find(' ') == -1:
                if name[6:-7] == 'PHI' or name[6:-7] == 'KAI':
                    name = 'GREEK SMALL LETTER ' + name[6:-7]
                else:
                    name = 'GREEK SMALL LETTER VAR' + name[6:-7]
            elif name == 'GREEK LETTER ARCHAIC KOPPA':
                name = 'GREEK CAPITAL LETTER VARKOPPA'
            elif name == 'GREEK SMALL LETTER ARCHAIC KOPPA':
                name = 'GREEK SMALL LETTER VARKOPPA'
            elif name[:13] == 'GREEK LETTER ' and \
                     name[13:].find(' ') == -1:
                name = 'GREEK CAPITAL LETTER ' + name[13:]
            if name[:21] == 'GREEK CAPITAL LETTER ':
                name = name[21:].capitalize()
                if name[:3] == 'Var':
                    name = name[:3] + name[3:].capitalize()
                name = '\\' + name
            elif name[:19] == 'GREEK SMALL LETTER ':
                name = name[19:].lower()
                name = '\\' + name
            else:
                name = None
            greek = True
        elif name[:6] == 'MUSIC ' and name [-5:] == ' SIGN':
            name = '\\' + name[6:-5].lower()
        elif name == 'SMILE' or name == 'FROWN':
            name = '\\' + name.lower()
        elif name[:27] == 'LATIN SMALL LETTER DOTLESS ' and \
             len(name) == 28:
            name = '\\' + name[27:].lower() + 'math'
        elif name == 'LEFTWARDS HARPOON WITH BARB UPWARDS':
            name = '\\leftharpoonup'
        elif name == 'LEFTWARDS HARPOON WITH BARB DOWNWARDS':
            name = '\\leftharpoondown'
        elif name == 'RIGHTWARDS HARPOON WITH BARB UPWARDS':
            name = '\\rightharpoonup'
        elif name == 'RIGHTWARDS HARPOON WITH BARB DOWNWARDS':
            name = '\\rightharpoondown'
        elif name == 'LEFTWARDS ARROW WITH HOOK':
            name = '\\hookleftarrow'
        elif name == 'RIGHTWARDS ARROW WITH HOOK':
            name = '\\hookrightarrow'
        elif name == 'STAR OPERATOR':
            name = '\\star'
        elif name == 'PARTIAL DIFFERENTIAL':
            name = '\\partial'
        elif name == 'SCRIPT SMALL L':
            name = '\\ell'
        elif name == 'SCRIPT CAPITAL P':
            name = '\\wp'
        elif name == 'DAGGER':
            name = '\\dagger'
        elif name == 'DOUBLE DAGGER':
            name = '\\ddagger'
        elif name == 'SECTION SIGN':
            name = '\\S'
        elif name == 'PILCROW SIGN':
            name = '\\P'
        elif name == 'LATIN SMALL LETTER D WITH STROKE':
            name = '\\dbar'
        else:
            name = None
        if name != None:
            table.append([name, code, greek])
    return table

def extract_math_alphabet_generic(unicode, name_prefix):
    digit_table = {'ZERO' : '0', 'ONE' : '1', 'TWO' : '2',
                   'THREE' : '3', 'FOUR' : '4', 'FIVE' : '5',
                   'SIX' : '6', 'SEVEN' : '7', 'EIGHT' : '8',
                   'NINE' : '9' }
    table = []
    for entry in unicode:
        code, name, general_category, canonical_combining_class, \
              bidi_class, decomposition_type, numeric_type_decimal, \
              numeric_type_digit, numeric_type_numeric, \
              bidi_mirrored, unicode_1_name, iso_comment, \
              simple_uppercase_mapping, simple_lowercase_mapping, \
              simple_titlecase_mapping = entry
        if name[:len(name_prefix[0]) + 1] == \
               name_prefix[0] + ' ' and \
               len(name) == len(name_prefix[0]) + 2:
            name = name[len(name_prefix[0]) + 1:].upper()
        elif name[:len(name_prefix[2]) + 1] == \
               name_prefix[2] + ' ' and \
               len(name) == len(name_prefix[2]) + 2:
            name = name[len(name_prefix[2]) + 1:].upper()
        elif name[:len(name_prefix[1]) + 1] == \
                 name_prefix[1] + ' ' and \
                 len(name) == len(name_prefix[1]) + 2:
                name = name[len(name_prefix[1]) + 1:].lower()
        elif name[:len(name_prefix[3]) + 1] == \
                 name_prefix[3] + ' ' and \
                 len(name) == len(name_prefix[3]) + 2:
            name = name[len(name_prefix[3]) + 1:].lower()
        elif name[:len(name_prefix[4]) + 1] == \
                 name_prefix[4] + ' ' and \
                 len(name) >= len(name_prefix[4]) + 2:
            name = digit_table[name[len(name_prefix[4]) + 1:]]
        else:
            name = None
        if name != None:
            table.append([name, code])
    return table

def extract_math_alphabet(unicode, name, alternate_name = None):
    if alternate_name == None:
        return extract_math_alphabet_generic(
            unicode,
            ['MATHEMATICAL ' + name + ' CAPITAL',
             'MATHEMATICAL ' + name + ' SMALL',
             name + ' CAPITAL',
             name + ' SMALL',
             'MATHEMATICAL ' + name + ' DIGIT'])
    else:
        return extract_math_alphabet_generic(
            unicode,
            ['MATHEMATICAL ' + name + ' CAPITAL',
             'MATHEMATICAL ' + name + ' SMALL',
             alternate_name + ' CAPITAL',
             alternate_name + ' SMALL',
             'MATHEMATICAL ' + name + ' DIGIT'])

def ucs_c_form(code):
    if isinstance(code, str):
        int_code = int(code, 16)
    elif isinstance(code, int):
        int_code = code
    if int_code == 0xd80d:
        return '%d' % int_code
    elif int_code == 0:
        return 'L\'\\%d\'' % int_code
    elif int_code < 128:
        return 'L\'\\%03o\'' % int_code
    elif int_code < 65536:
        return 'L\'\\u%04x\'' % int_code
    else:
        return 'L\'\\U%08x\'' % int_code

def escape_c_form(name):
    return name.replace('\\', '\\\\').replace('"', '\\"')
