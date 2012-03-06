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

#ifndef GLUON_PLOT2D_H_
#define GLUON_PLOT2D_H_

#include <stdint.h>
#include <iostream>
#include <gluon/geometry.h>
#include <gluon/color.h>
#include <gluon/rect.h>
#include <mathtext/mathtext.h>

namespace gluon {

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 869)
#endif // __INTEL_COMPILER
    class plot_2d_t {
	public:
		enum {
			ALIGN_NONE = 0,
			ALIGN_LOWER_LEFT,
			ALIGN_LOWER_CENTER,
			ALIGN_LOWER_RIGHT,
			ALIGN_CENTER_RIGHT,
			ALIGN_UPPER_RIGHT,
			ALIGN_UPPER_CENTER,
			ALIGN_UPPER_LEFT,
			ALIGN_CENTER_LEFT,
			ALIGN_CENTER,
			NALIGN
		};
    protected:
		rect_t _rect;
		bool _coordinate_matrix;
		std::pair<double, double> _horizontal_range;
		std::pair<double, double> _vertical_range;
		affine_transform_t _transform_logical_to_pixel;
		affine_transform_t _transform_pixel_to_logical;
		mathtext::math_text_t _horizontal_label;
		mathtext::math_text_t _vertical_label;
	private:
		static const int _ticks_size = 8;
		static const int _subticks_size = 4;
		bool _horizontal_pi_like;
		bool _vertical_pi_like;
		uint16_t _label_color;
		inline virtual rect_t rect(void) const
		{
			return rect_t();
		}
		inline virtual void begin(void)
		{
		}
		inline virtual void begin_unscissored(void)
		{
		}
		/**
		 * Determine if the number u is a "simple" rational multiple
		 * of pi, using the class of rational numbers. Simplicity is
		 * understood if the denominator in decimal representation
		 * does not exceed 2 digits.
		 *
		 * @param[in] u number to test for fmod(u, pi) being a simple
		 *              rational factor
		 * @return true if u can be factorized into a rational number
		 *         with denominator < 100 and pi, false otherwise
		 */
		bool is_pi_like(const float u) const;
		std::string label(const double u, const bool mod_pi = false);
		void margin(double u0, double u1, int &w_max, int &h_max,
					int &w_last, int &h_last,
					const bool pi_like = false);
		void axis(const double x0, const double y0,
				  const double x1, const double y1,
				  double u0, double u1,
				  const bool pi_like = false,
				  const int ticks_top_right = true);
    public:
		inline plot_2d_t(const rect_t &rect)
			: _rect(rect), _coordinate_matrix(false),
			  _horizontal_range(rect.left(), rect.right()),
			  _vertical_range(rect.bottom(), rect.top()),
			  _transform_logical_to_pixel(
					affine_transform_t::identity),
			  _transform_pixel_to_logical(
					affine_transform_t::identity)
		{
		}
		inline virtual ~plot_2d_t(void)
		{
		}
		inline virtual void end(void)
		{
		}
		inline virtual color_t get_color(void) const
		{

			return color_t();
		}
		inline virtual void set_color(const color_t color)
		{
		}
		inline virtual void
		line(const float x0, const float y0,
			 const float x1, const float y1) const
		{
		}
		inline virtual void
		filled_rectangle(const float x, const float y,
						 const float width, const float height)
			const
		{
		}
		inline virtual void filled_polygon(const polygon_t &p) const
		{
		}
		inline virtual float font_size(void) const
		{
			return 0.0F;
		}
		inline virtual void text(const float x, const float y,
								 const unsigned int alignment,
								 const mathtext::math_text_t &math_text,
								 const float distance = 0)
		{
		}
		// Range
		void set_range(const double u0, const double u1,
					   const double v0, const double v1);
		inline void set_horizontal_label(const mathtext::math_text_t &label)
		{
			_horizontal_label = label;
		}
		inline void set_vertical_label(const mathtext::math_text_t &label)
		{
			_vertical_label = label;
		}
		void frame(void);
    };
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

}

#endif // GLUON_PLOT2D_H_
