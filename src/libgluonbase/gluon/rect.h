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

#ifndef GLUON_RECT_H_
#define GLUON_RECT_H_

#include <stdint.h>
#ifdef HAVE_SDL
#include <SDL/SDL.h>
#endif // HAVE_SDL

namespace gluon {

	class rect_t {
	protected:
		float _left;
		float _bottom;
		float _width;
		float _height;
	public:
		inline rect_t(void)
		{
		}
		inline rect_t(const float left, const float bottom,
					  const float width, const float height)
			: _left(left), _bottom(bottom), _width(width),
			  _height(height)
		{
		}
		inline float left(void) const
		{
			return _left;
		}
		inline float &left(void)
		{
			return _left;
		}
		inline float bottom(void) const
		{
			return _bottom;
		}
		inline float &bottom(void)
		{
			return _bottom;
		}
		inline float width(void) const
		{
			return _width;
		}
		inline float &width(void)
		{
			return _width;
		}
		inline float height(void) const
		{
			return _height;
		}
		inline float &height(void)
		{
			return _height;
		}
		inline virtual float right(void) const
		{
			return _left + _width;
		}
		inline virtual float top(void) const
		{
			return _bottom + _height;
		}
		inline virtual rect_t scissor(const rect_t rect) const
		{
			rect_t scissored_rect = rect;

			if (scissored_rect._left < _left) {
				scissored_rect._width +=
					scissored_rect._left - _left;
				scissored_rect._left = _left;
			}
			if (scissored_rect._bottom < _bottom) {
				scissored_rect._height +=
					scissored_rect._bottom - _bottom;
				scissored_rect._bottom = _bottom;
			}
			if (scissored_rect._width > _width)
				scissored_rect._width = _width;
			if (scissored_rect._height > _height)
				scissored_rect._height = _height;

			return scissored_rect;
		}
	};

#ifdef HAVE_SDL
	class screen_rect_t : public SDL_Rect {
	public:
		inline screen_rect_t(void)
		{
		}
		inline screen_rect_t(const int16_t left, const int16_t top,
							 const uint16_t width,
							 const uint16_t height)
		{
			x = left;
			y = top;
			w = width;
			h = height;
		}
		inline screen_rect_t(const rect_t &rect)
		{
			x = rect.left();
			y = rect.top();
			w = rect.width();
			h = rect.height();
		}
		inline int16_t left(void) const
		{
			return x;
		}
		inline int16_t &left(void)
		{
			return x;
		}
		inline int16_t top(void) const
		{
			return y;
		}
		inline int16_t &top(void)
		{
			return y;
		}
		inline uint16_t width(void) const
		{
			return w;
		}
		inline uint16_t &width(void)
		{
			return w;
		}
		inline uint16_t height(void) const
		{
			return h;
		}
		inline uint16_t &height(void)
		{
			return h;
		}
		inline virtual int32_t right(void) const
		{
			return static_cast<int32_t>(x) + static_cast<int32_t>(w);
		}
		inline virtual int32_t bottom(void) const
		{
			return static_cast<int32_t>(y) + static_cast<int32_t>(h);
		}
		inline virtual screen_rect_t
		scissor(const screen_rect_t rect) const
		{
			screen_rect_t scissored_rect = rect;

			if (scissored_rect.x < x) {
				scissored_rect.w += scissored_rect.x - x;
				scissored_rect.x = x;
			}
			if (scissored_rect.y < y) {
				scissored_rect.h += scissored_rect.y - y;
				scissored_rect.y = y;
			}
			if (scissored_rect.w > w)
				scissored_rect.w = w;
			if (scissored_rect.h > h)
				scissored_rect.h = h;

			return scissored_rect;
		}
	};
#endif // HAVE_SDL

}

#endif // GLUON_RECT_H_
