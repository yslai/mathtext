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

import os, sys

def capability():
    system = None
    architecture = None
    cpuinfo = {}
    uname_s_pipe = os.popen('uname -s 2>&1', 'r')
    try:
        system = uname_s_pipe.read().split('\n')[0]
    finally:
        uname_s_pipe.close()
    uname_p_pipe = os.popen('uname -p 2>&1', 'r')
    try:
        architecture = uname_p_pipe.read().split('\n')[0]
    finally:
        uname_p_pipe.close()
    if system == 'Linux' and os.path.isfile('/proc/cpuinfo'):
        cpuinfo_pipe = open('/proc/cpuinfo', 'r')
        try:
            for line in cpuinfo_pipe.read().split('\n'):
                if line.find(': ') != -1:
                    key, value = line.split(': ')
                    key = key.split('\t')[0]
                    cpuinfo[key] = value
        finally:
            cpuinfo_pipe.close()
    capability = []
    if architecture[:1] == 'i' and architecture[-2:] == '86' or \
           architecture in ['x86', 'x86pc', 'k5', 'k6', 'k6-2',
                            'k6-3', 'i586-i686'] or \
                            architecture[:7] == 'pentium' or \
                            architecture[:6] == 'athlon':
        if cpuinfo.has_key('vendor_id') and \
               cpuinfo.has_key('cpu family') and \
               cpuinfo.has_key('model'):
            if cpuinfo['vendor_id'] == 'AuthenticAMD':
                # FIXME: 32-bit Athlon XP (SSE) cannot be
                # distinguished this way from 32-bit Athlon, but I do
                # not have any of these around.
                if cpuinfo['cpu family'] == '15':
                    capability += ['HAVE_SSE', 'HAVE_SSE2']
            elif cpuinfo['vendor_id'] == 'GenuineIntel':
                if cpuinfo['cpu family'] == '6':
                    if int(cpuinfo['model']) >= 15:
                        # 32-bit Core 2
                        capability += ['HAVE_SSE', 'HAVE_SSE2',
                                       'HAVE_SSE3', 'HAVE_SSSE3']
                    elif cpuinfo['model'] == '9' or \
                             int(cpuinfo['model']) >= 13:
                        # 32-bit Pentium M
                        capability += ['HAVE_SSE', 'HAVE_SSE2']
                    elif int(cpuinfo['model']) >= 7:
                        # 32-bit Pentium III
                        capability += ['HAVE_SSE']
                elif cpuinfo['cpu family'] == '15':
                    # 32-bit Nocoa or Prescott
                    capability += ['HAVE_SSE', 'HAVE_SSE2']
    elif architecture in ['x86_64', 'amd64']:
        if cpuinfo.has_key('vendor_id') and \
               cpuinfo.has_key('cpu family'):
            if cpuinfo['vendor_id'] == 'AuthenticAMD':
                capability += ['HAVE_SSE', 'HAVE_SSE2']
            elif cpuinfo['vendor_id'] == 'GenuineIntel':
                capability += ['HAVE_SSE', 'HAVE_SSE2']
                if cpuinfo['cpu family'] == '6':
                    # 64-bit Core 2
                    capability += ['HAVE_SSE3', 'HAVE_SSSE3']
    return capability
