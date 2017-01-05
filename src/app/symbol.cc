// mathtext - A TeX/LaTeX compatible rendering library. Copyright (C)
// 2008-2012 Yue Shi Lai <ylai@users.sourceforge.net>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1 of
// the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301 USA

#include <cstdio>
#include <cmath>
#include <vector>
#include <list>
#include <gluon/screen.h>
#include <mathtext/mathtext.h>

namespace {

	std::wstring bad_cast(const std::string string)
	{
		std::wstring wstring;

		for (std::string::const_iterator iterator = string.begin();
			iterator != string.end(); iterator++) {
			wstring.push_back(*iterator);
		}

		return wstring;
	}
}

int main(int argc, char *argv[])
{
	gluon::screen_t screen(1280, 800);

	screen.open_font("data/font/MyriadPro");
	screen.begin();

	const char *symbol[] = {
"\\hookleftarrow",
"\\hookrightarrow",
"!",
"(",
")",
",",
".",
"/",
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"<",
">",
"?",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
"[",
"]",
"a",
"b",
"c",
"d",
"e",
"f",
"g",
"h",
"i",
"j",
"k",
"l",
"m",
"n",
"o",
"p",
"q",
"r",
"s",
"t",
"u",
"v",
"w",
"x",
"y",
"z",
"\\S",
"\\P",
"\\dbar",
"\\updbar",
"\\imath",
"\\jmath",
"\\Gamma",
"\\upGamma",
"\\Delta",
"\\upDelta",
"\\Theta",
"\\upTheta",
"\\Lambda",
"\\upLambda",
"\\Xi",
"\\upXi",
"\\Pi",
"\\upPi",
"\\Sigma",
"\\upSigma",
"\\Upsilon",
"\\upUpsilon",
"\\Phi",
"\\upPhi",
"\\Psi",
"\\upPsi",
"\\Omega",
"\\upOmega",
"\\alpha",
"\\upalpha",
"\\beta",
"\\upbeta",
"\\gamma",
"\\upgamma",
"\\delta",
"\\updelta",
"\\varepsilon",
"\\upvarepsilon",
"\\zeta",
"\\upzeta",
"\\eta",
"\\upeta",
"\\theta",
"\\uptheta",
"\\iota",
"\\upiota",
"\\kappa",
"\\upkappa",
"\\lambda",
"\\uplambda",
"\\mu",
"\\upmu",
"\\nu",
"\\upnu",
"\\xi",
"\\upxi",
"\\pi",
"\\uppi",
"\\rho",
"\\uprho",
"\\varsigma",
"\\upvarsigma",
"\\sigma",
"\\upsigma",
"\\tau",
"\\uptau",
"\\upsilon",
"\\upupsilon",
"\\varphi",
"\\upvarphi",
"\\chi",
"\\upchi",
"\\psi",
"\\uppsi",
"\\omega",
"\\upomega",
"\\varbeta",
"\\upvarbeta",
"\\vartheta",
"\\upvartheta",
"\\phi",
"\\upphi",
"\\varpi",
"\\upvarpi",
"\\VarKoppa",
"\\upVarKoppa",
"\\varkoppa",
"\\upvarkoppa",
"\\Stigma",
"\\upStigma",
"\\stigma",
"\\upstigma",
"\\Digamma",
"\\upDigamma",
"\\digamma",
"\\updigamma",
"\\Koppa",
"\\upKoppa",
"\\koppa",
"\\upkoppa",
"\\Sampi",
"\\upSampi",
"\\sampi",
"\\upsampi",
"\\varkappa",
"\\upvarkappa",
"\\varrho",
"\\upvarrho",
"\\epsilon",
"\\upepsilon",
"\\Sho",
"\\upSho",
"\\sho",
"\\upsho",
"\\San",
"\\upSan",
"\\san",
"\\upsan",
"\\dagger",
"\\ddagger",
"\\ell",
"\\wp",
"\\leftharpoonup",
"\\leftharpoondown",
"\\rightharpoonup",
"\\rightharpoondown",
"\\partial",
"\\star",
"\\frown",
"\\smile",
"\\flat",
"\\natural",
"\\sharp"
	};

	screen.set_point_size(2);

	for (size_t i = 0; i < 208; i++) {
		screen.set_font_size(48);
		screen.text(16 + 64 * (i % 20), 720 - 64 * (i / 20), 0, mathtext::math_text_t(symbol[i]), 0);
		screen.set_font_size(12);
		screen.text(16 + 64 * (i % 20), 720 - 64 * (i / 20) - 16, bad_cast(symbol[i]), 0);
	}

	screen.end();
	screen.update();
	screen.pause();

	return 0;
}
