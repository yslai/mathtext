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

#include <cmath>
#include <algorithm>
#include <iostream>
#include <gluon/color.h>

/////////////////////////////////////////////////////////////////////

namespace gluon {

#ifdef HAVE_LCMS
	cmsHPROFILE color_t::_lab_profile = NULL;
	cmsHTRANSFORM color_t::_lab_to_display_transform = NULL;
	cmsHTRANSFORM color_t::_display_to_lab_transform = NULL;
	cmsHPROFILE color_t::_srgb_profile = NULL;
	cmsHTRANSFORM color_t::_srgb_to_display_transform = NULL;
	cmsHTRANSFORM color_t::_display_to_srgb_transform = NULL;

	void color_t::set_profile(cmsHPROFILE display_profile,
							  unsigned int intent)
	{
		if (display_profile == NULL)
			return;
		_lab_profile = cmsCreateLabProfile(NULL);
		if (_lab_profile != NULL) {
			_lab_to_display_transform =
				cmsCreateTransform(_lab_profile, TYPE_Lab_DBL,
								   display_profile, TYPE_RGB_DBL,
								   intent, 0);
			_display_to_lab_transform =
				cmsCreateTransform(display_profile, TYPE_RGB_DBL,
								   _lab_profile, TYPE_Lab_DBL,
								   intent, 0);
		}
		_srgb_profile = cmsCreate_sRGBProfile();
		if (_srgb_profile != NULL) {
			_srgb_to_display_transform =
				cmsCreateTransform(_srgb_profile, TYPE_RGB_DBL,
								   display_profile, TYPE_RGB_DBL,
								   intent, 0);
			_display_to_srgb_transform =
				cmsCreateTransform(display_profile, TYPE_RGB_DBL,
								   _srgb_profile, TYPE_RGB_DBL,
								   intent, 0);
		}
	}
#endif // HAVE_LCMS

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 810)
#endif // __INTEL_COMPILER
	void color_t::transform(void)
	{
#ifdef HAVE_LCMS
		if (_color_space == COLOR_SPACE_LAB &&
		   _lab_to_display_transform != NULL) {
			double buffer_from[NCOLOR_MAX];

			buffer_from[0] = _color[0];
			buffer_from[1] = _color[1];
			buffer_from[2] = _color[2];

			double buffer_to[3];

			cmsDoTransform(_lab_to_display_transform,
						   buffer_from, buffer_to, 1);
			_display_color[0] = buffer_to[0];
			_display_color[1] = buffer_to[1];
			_display_color[2] = buffer_to[2];
			return;
		}
		else if (_color_space == COLOR_SPACE_RGB &&
				_srgb_to_display_transform != NULL) {
			double buffer_from[NCOLOR_MAX];

			buffer_from[0] = _color[0];
			buffer_from[1] = _color[1];
			buffer_from[2] = _color[2];

			double buffer_to[3];

			cmsDoTransform(_srgb_to_display_transform,
						   buffer_from, buffer_to, 1);
			_display_color[0] = buffer_to[0];
			_display_color[1] = buffer_to[1];
			_display_color[2] = buffer_to[2];
			return;
		}
#endif // HAVE_LCMS
		if (_color_space == COLOR_SPACE_RGB) {
			_display_color[0] = _color[0];
			_display_color[1] = _color[1];
			_display_color[2] = _color[2];
		}
		else {
			_display_color[0] = 0.0F;
			_display_color[1] = 0.0F;
			_display_color[2] = 0.0F;
		}
	}
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

}
