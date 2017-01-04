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

#ifndef GLUON_SCREEN_H_
#define GLUON_SCREEN_H_

#include <stdint.h>
#ifdef HAVE_OPENGL
#include <GL/gl.h>
#endif // HAVE_OPENGL
#ifdef HAVE_LCMS
#include <lcms.h>
#else // HAVE_LCMS
#ifdef HAVE_LCMS2
#include <lcms2.h>
#endif // HAVE_LCMS2
#endif // HAVE_LCMS
#include <gluon/rect.h>
#include <gluon/surface.h>

namespace gluon {

#ifdef HAVE_OPENGL
	class screen_t : public opengl_surface_t {
	private:
		screen_rect_t _screen_rect;
		SDL_Surface *_sdl_surface;
		bool _sdl_fullscreen;
		SDL_Event _event;
		bool is_interrupt(const SDL_keysym keysym) const;
		void reshape(void) const;
	public:
		screen_t(const int32_t width = 1280,
				 const int32_t height = 720,
				 const bool fullscreen = false);
		~screen_t(void);
		inline void set_caption(const std::string &title,
								const std::string &icon_title) const
		{
			SDL_WM_SetCaption(title.c_str(), icon_title.c_str());
		}
		void clear(void) const;
		void flush(void) const;
		/////////////////////////////////////////////////////////////
		// Events and delays
		inline SDL_Event event(void) const
		{
			return _event;
		}
		inline char *event_key_name(void) const
		{
			return SDL_GetKeyName(_event.key.keysym.sym);
		}
		inline bool wait_event(void)
		{
			return SDL_WaitEvent(&_event) != 0;
		}
		inline bool poll_event(void)
		{
			return SDL_PollEvent(&_event) != 0;
		}
		/**
		 * Wait t seconds
		 *
		 * @param[in] t number of seconds to wait
		 */
		inline void delay(const double t) const
		{
			SDL_Delay((int)(t * 1000));
		}
		void check_interrupt(void);
		/**
		 * Wait until a key or mouse button stroke
		 */
		void pause(void);
	};
#endif // HAVE_OPENGL

}

#endif // GLUON_SCREEN_H_
