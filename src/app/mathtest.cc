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

int main(int argc, char *argv[])
{
	const char *string = "\\prod_{j\\ge0} \\left(\\sum_{k\\ge0} a_{jk}z^k\\right) = \\sum_{n\\ge0} z^n \\left(\\sum_{k_0,k_1,\\ldots\\ge0\\atop k_0+k_1+\\cdots=n} a_{0k_0}a_{1k_1} \\cdots \\right)";

	if(argc > 1) {
		string = argv[1];
	}

	mathtext::math_text_t text(string);

	text.render_structure() = true;

	gluon::screen_t screen(1280, 360);

	screen.open_font("data/font/Arimo");
	screen.set_font_size(64);
	screen.set_point_size(2);
	screen.begin();

	const float baseline = 180;

	screen.line(32, baseline, screen.rect().right(), baseline);
	screen.line(32, baseline + screen.default_axis_height(),
				screen.rect().right(),
				baseline + screen.default_axis_height());
	screen.text(32, baseline, 0, text, 0);
	screen.end();
	screen.update();
	screen.pause();

	return 0;
}
