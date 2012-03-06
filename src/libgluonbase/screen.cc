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

#include <iostream>
#include <gluon/screen.h>

/////////////////////////////////////////////////////////////////////

namespace gluon {

#ifdef HAVE_OPENGL
	bool screen_t::is_interrupt(const SDL_keysym keysym) const
	{
		return keysym.sym == SDLK_ESCAPE ||
			((keysym.sym == SDLK_c || keysym.sym == SDLK_q) &&
			 (keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0);
	}

	void screen_t::reshape(void) const
	{
		glViewport(_screen_rect.left(), _screen_rect.top(),
				   _screen_rect.width(), _screen_rect.height());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-1.0F, -1.0F, 0.0F);
		glScalef(2.0F / (GLfloat)_screen_rect.width(),
				 2.0F / (GLfloat)_screen_rect.height(), 1.0F);
	}

	screen_t::screen_t(const int32_t width, const int32_t height,
					   const bool fullscreen)
		: opengl_surface_t(rect_t(0, 0, width, height), true),
		  _screen_rect(0, 0, width, height)
	{
		// Initialize SDL
		if(SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: unable to initialize SDL: "
					  << SDL_GetError() << std::endl;
			return;
		}

		// Create a set of reasonable video mode flags
		std::vector<Uint32> flag;

		if(fullscreen)
			flag.push_back(SDL_OPENGL | SDL_FULLSCREEN);
		flag.push_back(SDL_OPENGL);

		// Try to initialize the video mode using the set of flags
		for(std::vector<Uint32>::const_iterator iterator =
				flag.begin(); iterator != flag.end(); iterator++) {
			_sdl_surface =
				SDL_SetVideoMode(_screen_rect.width(),
								 _screen_rect.height(),
								 _depth, *iterator);
			if(_sdl_surface)
				_sdl_fullscreen =
					((*iterator & SDL_FULLSCREEN) != 0);
				break;
		}
		if(_sdl_surface == NULL) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: unable to set video mode: "
					  << SDL_GetError() << std::endl;
			return;
		}

		// Set the OpenGL flags
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_SCISSOR_TEST);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

		// Set the foreground and background colors
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(0, 0, 0);

		reshape();
		generate_display_list();

		_event.type = SDL_NOEVENT;
	}

	screen_t::~screen_t()
	{
		if(_sdl_surface) {
			SDL_FreeSurface(_sdl_surface);
			// This will prevent ~opengl_surface_t() to perform a
			// duplicate deallocation.
			_sdl_surface = NULL;
			SDL_Quit();
		}
	}

	void screen_t::clear(void) const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void screen_t::flush(void) const
	{
		SDL_GL_SwapBuffers();
	}

	void screen_t::check_interrupt(void)
	{
		if(!poll_event())
			return;
		if(_event.type == SDL_KEYDOWN &&
		   is_interrupt(_event.key.keysym)) {
			SDL_Quit();
			exit(0);
		}
	}

	void screen_t::pause(void)
	{
		while(true) {
			while(poll_event()) {
				switch(_event.type) {
				case SDL_VIDEORESIZE:
					_sdl_surface =
						SDL_SetVideoMode(_event.resize.w,
										 _event.resize.h, _depth,
										 SDL_OPENGL | SDL_RESIZABLE);
					if(_sdl_surface) {
						_screen_rect.w = _sdl_surface->w;
						_screen_rect.h = _sdl_surface->h;
						reshape();
					}
					break;
				case SDL_VIDEOEXPOSE:
					clear();
					update();
					flush();
					break;
				case SDL_KEYDOWN:
					if(is_interrupt(_event.key.keysym)) {
						SDL_Quit();
						exit(0);
					}
					if(_event.key.keysym.sym == SDLK_RETURN ||
					   _event.key.keysym.sym == SDLK_SPACE ||
					   _event.key.keysym.sym == SDLK_KP_ENTER)
						return;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(event().button.button == SDL_BUTTON_LEFT)
						return;
					break;
				case SDL_QUIT:
					SDL_Quit();
					exit(0);
				}
			}
			SDL_Delay(10);
		}
	}
#endif // HAVE_OPENGL

}
