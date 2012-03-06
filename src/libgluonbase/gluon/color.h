// -*- mode: c++; -*-

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

#ifndef GLUON_COLOR_H_
#define GLUON_COLOR_H_

#include <vector>
#include <stdint.h>
#ifdef HAVE_OPENGL
#include <GL/gl.h>
#endif // HAVE_OPENGL
#ifdef HAVE_LCMS
#include <lcms.h>
#endif // HAVE_LCMS

namespace gluon {

#ifdef HAVE_LCMS
	typedef cmsCIEXYZ ciexyz_t;
	typedef cmsCIELab cielab_t;
#endif // HAVE_LCMS

	class screen_t;

	class color_t {
	public:
		enum {
			// A RGB color space
			COLOR_SPACE_DISPLAY_RGB = 0,
			// The CIE 1976 L*a*b* color space
			COLOR_SPACE_LAB,
			// A RGB color space
			COLOR_SPACE_RGB,
			// A CMYK color space
			COLOR_SPACE_CMYK,
			// A CMYKOG (Hexachrome(R)) color space
			COLOR_SPACE_CMYKOG,
			NCOLOR_SPACE
		};
		enum {
			NCOLOR_MAX = 4
		};
	private:
#ifdef HAVE_LCMS
		static cmsHPROFILE _lab_profile;
		static cmsHTRANSFORM _lab_to_display_transform;
		static cmsHTRANSFORM _display_to_lab_transform;
		static cmsHPROFILE _srgb_profile;
		static cmsHTRANSFORM _srgb_to_display_transform;
		static cmsHTRANSFORM _display_to_srgb_transform;
#endif // HAVE_LCMS
		unsigned int _color_space;
		float _color[NCOLOR_MAX];
#ifdef HAVE_OPENGL
		GLfloat _display_color[3];
#else // HAVE_OPENGL
		float _display_color[3];
#endif // HAVE_OPENGL
		void transform(void);
	public:
#ifdef HAVE_LCMS
		static void set_profile(
			cmsHPROFILE display_profile,
			unsigned int intent = INTENT_RELATIVE_COLORIMETRIC);
#endif // HAVE_LCMS
		inline color_t(void)
		{
		}
		inline color_t(const unsigned int color_space,
					   const float c0, const float c1,
					   const float c2)
			: _color_space(color_space)
		{
			_color[0] = c0;
			_color[1] = c1;
			_color[2] = c2;
			if(_color_space == COLOR_SPACE_DISPLAY_RGB) {
				_display_color[0] = c0;
				_display_color[1] = c1;
				_display_color[2] = c2;
			}
			else
				transform();
		}
		inline color_t(const unsigned int color_space,
					   const float c0, const float c1,
					   const float c2, const float c3)
			: _color_space(color_space)
		{
			_color[0] = c0;
			_color[1] = c1;
			_color[2] = c2;
			_color[3] = c3;
			transform();
		}
		inline float red(void) const
		{
			return _display_color[0];
		}
		inline float green(void) const
		{
			return _display_color[1];
		}
		inline float blue(void) const
		{
			return _display_color[2];
		}
		inline color_t operator*(const float scale) const
		{
			return color_t(
				COLOR_SPACE_RGB,
				_display_color[0] * scale,
				_display_color[1] * scale,
				_display_color[2] * scale);
		}
		inline color_t operator+(const color_t &color) const
		{
			return color_t(
				COLOR_SPACE_RGB,
				_display_color[0] + color._display_color[0],
				_display_color[1] + color._display_color[1],
				_display_color[2] + color._display_color[2]);
		}
		friend color_t operator*(const float, const color_t);
	};
	inline color_t operator*(const float scale, const color_t color)
	{
		return color_t(
			color_t::COLOR_SPACE_RGB,
			scale * color._display_color[0],
			scale * color._display_color[1],
			scale * color._display_color[2]);
	}

}

#endif // GLUON_COLOR_H_
