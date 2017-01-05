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
#include <iostream>
#include <algorithm>
#include <gluon/screen.h>

/////////////////////////////////////////////////////////////////////

namespace gluon {

#ifdef HAVE_OPENGL
	void opengl_surface_t::generate_display_list(void)
	{
		if (_display_list == 0) {
			delete_display_list();
		}
		_display_list = glGenLists(NDISPLAY_LIST);
	}

	void opengl_surface_t::delete_display_list(void)
	{
		if (_display_list != 0) {
			glDeleteLists(_display_list, NDISPLAY_LIST);
		}
	}

	void opengl_surface_t::begin_coordinate_matrix(void)
	{
		glPushMatrix();
		if (_coordinate_matrix) {
			glTranslatef(static_cast<GLfloat>(_rect.left()),
						 static_cast<GLfloat>(_rect.bottom()),
						 0.0F);
			glScalef(_transform_logical_to_pixel[0],
					 _transform_logical_to_pixel[3], 1.0F);
			glTranslated(-_horizontal_range.first,
						 -_vertical_range.first, 0.0);
		}
	}

	opengl_surface_t::
	opengl_surface_t(const rect_t &rect, const bool master, 
					 const int16_t depth)
		: surface_t(rect, master), _depth(depth),
		  _display_list(0)
	{
		std::fill(_font, _font + NFAMILY,
				  reinterpret_cast<FTFont *>(NULL));
		if (!master) {
			// The master must delay generating the display list.
			generate_display_list();
		}
	}

	opengl_surface_t::opengl_surface_t(opengl_surface_t &surface)
		: surface_t(surface), _depth(surface._depth),
		  _display_list(0)
	{
		std::fill(_font, _font + NFAMILY,
				  reinterpret_cast<FTFont *>(NULL));
		surface._subsurface.push_back(this);
		generate_display_list();
	}

	opengl_surface_t::~opengl_surface_t(void)
	{
		for (unsigned int family = 0; family < NFAMILY; family++) {
			if (_font[family] != NULL) {
				delete _font[family];
				_font[family] = NULL;
			}
		}
	}

	opengl_surface_t &opengl_surface_t::
	surface(const rect_t &rect)
	{
		const rect_t scissored_rect = _rect.scissor(rect);
		opengl_surface_t *ret =
			new opengl_surface_t(scissored_rect, false);

		_subsurface.push_back(ret);
		ret->_supersurface = this;

		return *ret;
	}

	opengl_surface_t &opengl_surface_t::
	surface(const short x, const short y,
			const unsigned short width,
			const unsigned short height)
	{
		return surface(rect_t(x, y, width, height));
	}

	void opengl_surface_t::begin_unscissored(void)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		begin_coordinate_matrix();
		unscissor();
		// FTGLPixmapFont requires GL_COMPILE_AND_EXECUTE in order to
		// switch colors
		glNewList(_display_list + DISPLAY_LIST_UNSCISSORED,
				  GL_COMPILE_AND_EXECUTE);
	}

	void opengl_surface_t::scissor(void) const
	{
		glScissor(_rect.left(), _rect.bottom(),
				  _rect.width(), _rect.height());
	}

	void opengl_surface_t::unscissor(void) const
	{
		rect_t rect = screen_rect();

		glScissor(rect.left(), rect.bottom(),
				  rect.width(), rect.height());
	}

	void opengl_surface_t::begin(void)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		begin_coordinate_matrix();
		scissor();
		// FTGLPixmapFont requires GL_COMPILE_AND_EXECUTE in order to
		// switch colors
		glNewList(_display_list + DISPLAY_LIST_SCISSORED,
				  GL_COMPILE_AND_EXECUTE);
	}

	void opengl_surface_t::end(void)
	{
		glEndList();
		unscissor();
		end_coordinate_matrix();
		glPopAttrib();
	}

	void opengl_surface_t::update(void)
	{
		begin_coordinate_matrix();
		scissor();
		glCallList(_display_list + DISPLAY_LIST_SCISSORED);
		unscissor();
		end_coordinate_matrix();
		for (std::vector<surface_t *>::iterator iterator =
				_subsurface.begin();
			 iterator != _subsurface.end(); iterator++) {
			if (*iterator != NULL) {
				(*iterator)->update();
			}
		}
		glCallList(_display_list + DISPLAY_LIST_UNSCISSORED);
	}

	color_t opengl_surface_t::get_color(void) const
	{
		GLfloat parameter_rgba[4];

		glGetFloatv(GL_CURRENT_COLOR, parameter_rgba);

		const float red = parameter_rgba[0];
		const float green = parameter_rgba[1];
		const float blue = parameter_rgba[2];

		return color_t(color_t::COLOR_SPACE_DISPLAY_RGB,
					   red, green, blue);
	}

	void opengl_surface_t::get_rgb_color(float &red, float &green,
										 float &blue) const
	{
		GLfloat parameter_rgba[4];

		glGetFloatv(GL_CURRENT_COLOR, parameter_rgba);
		red = parameter_rgba[0];
		green = parameter_rgba[1];
		blue = parameter_rgba[2];
	}

	void opengl_surface_t::
	get_rgba_color(float &red, float &green, float &blue,
				   float &alpha) const
	{
		GLfloat parameter_rgba[4];

		glGetFloatv(GL_CURRENT_COLOR, parameter_rgba);
		red = parameter_rgba[0];
		green = parameter_rgba[1];
		blue = parameter_rgba[2];
		alpha = parameter_rgba[3];
	}

	/////////////////////////////////////////////////////////////////
	// Graphics primitives

	void opengl_surface_t::point(const float x, const float y) const
	{
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
	}

	void opengl_surface_t::line(const float x0, const float y0,
								const float x1, const float y1) const
	{
		glBegin(GL_LINES);
		glVertex2f(x0, y0);
		glVertex2f(x1, y1);
		glEnd();
	}

	void opengl_surface_t::
	rectangle(const float x, const float y,
			  const float width, const float height) const
	{
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();
	}

	void opengl_surface_t::
	rectangle(const mathtext::bounding_box_t &bounding_box) const
	{
		glBegin(GL_LINE_LOOP);
		glVertex2f(bounding_box.left(), bounding_box.bottom());
		glVertex2f(bounding_box.right(), bounding_box.bottom());
		glVertex2f(bounding_box.right(), bounding_box.top());
		glVertex2f(bounding_box.left(), bounding_box.top());
		glEnd();
	}

	void opengl_surface_t::
	filled_rectangle(const float x, const float y,
					 const float width, const float height) const
	{
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();
	}

	void opengl_surface_t::
	filled_rectangle(const mathtext::bounding_box_t &bounding_box)
		const
	{
		glBegin(GL_QUADS);
		glVertex2f(bounding_box.left(), bounding_box.bottom());
		glVertex2f(bounding_box.right(), bounding_box.bottom());
		glVertex2f(bounding_box.right(), bounding_box.top());
		glVertex2f(bounding_box.left(), bounding_box.top());
		glEnd();
	}

	void opengl_surface_t::polygon(const polygon_t &p) const
	{
		if (p.empty()) {
			return;
		}
		glBegin(GL_LINE_LOOP);
		for (unsigned long i = 0; i < p.size(); i++) {
			glVertex2f(p[i][0], p[i][1]);
		}
		glEnd();
	}

	void opengl_surface_t::filled_polygon(const polygon_t &p) const
	{
		if (p.empty()) {
			return;
		}
		glBegin(GL_POLYGON);
		for (unsigned long i = 0; i < p.size(); i++) {
			glVertex2f(p[i][0], p[i][1]);
		}
		glEnd();
	}

	void opengl_surface_t::
	raster(const float x, const float y,
		   const float width, const float height,
		   const float density[],
		   const int nhorizontal, const int nvertical) const
	{
		const float dx = width / nhorizontal *
			fabsf(_transform_logical_to_pixel[0]);
		const float dy = height / nvertical *
			fabsf(_transform_logical_to_pixel[3]);

		glPushMatrix();
		glRasterPos2f(x, y);
		glPixelZoom(dx, dy);
		glDrawPixels(nhorizontal, nvertical, GL_RGBA, GL_FLOAT,
					 reinterpret_cast<const GLvoid *>(density));
		glPixelZoom(1.0F, 1.0F);
		glPopMatrix();
	}

	void opengl_surface_t::
	text_raw(const float x, const float y, const std::wstring string,
			 const unsigned int family)
	{
		if (family >= NFAMILY || _font[family] == NULL) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: font not initialized"
					  << std::endl;
			return;
		}
		fprintf(stderr, "%u U+%04X %f %f\n", family, string[0], x, y);
		glPushMatrix();
		glRasterPos2f(x, y);
		_font[family]->Render(string.c_str());
		glPopMatrix();
	}

	/////////////////////////////////////////////////////////////////
	// Typography

	void opengl_surface_t::
	open_font_overwrite(const std::string &filename,
						const unsigned int family)
	{
		if (_font[family] != NULL) {
			delete _font[family];
		}
#ifdef FTGL_BITMAP_FONT
		_font[family] = new FTGLBitmapFont(filename.c_str());
#else // FTGL_BITMAP_FONT
		_font[family] = new FTGLPixmapFont(filename.c_str());
#endif // FTGL_BITMAP_FONT
		_font[family]->CharMap(ft_encoding_unicode);
		_font[family]->UseDisplayList(false);
	}

	void opengl_surface_t::
	open_font_default(const std::string &filename,
					  const unsigned int family)
	{
		if (_font[family] == NULL) {
			open_font_overwrite(filename, family);
		}
	}

	void opengl_surface_t::
	set_font_size(const float size, const unsigned int family)
	{
		if (_font[family] != NULL) {
			_font[family]->FaceSize(size);
		}
	}

	void opengl_surface_t::set_font_size(const float size)
	{
		_font_size = size;
		for (unsigned int family = 0; family < NFAMILY; family++) {
			if (_font[family] != NULL) {
				_font[family]->FaceSize(size);
			}
		}
	}

	void opengl_surface_t::reset_font_size(const unsigned int family)
	{
		if (family < NFAMILY && _font[family] != NULL) {
			_font[family]->FaceSize(_font_size);
		}
	}

	mathtext::bounding_box_t opengl_surface_t::
	bounding_box(const std::wstring string,
				 const unsigned int family)
	{
		if (family >= NFAMILY || _font[family] == NULL) {
			return mathtext::bounding_box_t(0, 0, 0, 0, 0, 0);
		}

		float lower_left_near_x;
		float lower_left_near_y;
		float lower_left_near_z;
		float upper_right_far_x;
		float upper_right_far_y;
		float upper_right_far_z;

		_font[family]->BBox(string.c_str(), lower_left_near_x,
							lower_left_near_y, lower_left_near_z,
							upper_right_far_x, upper_right_far_y,
							upper_right_far_z);

		const float advance =
			_font[family]->Advance(string.c_str());
		const float margin = std::max(0.0F, lower_left_near_x);
		const float italic_correction =
			upper_right_far_x <= advance ? 0.0F :
			std::max(0.0F, upper_right_far_x + margin - advance);

		return mathtext::bounding_box_t(lower_left_near_x,
										lower_left_near_y,
										upper_right_far_x,
										upper_right_far_y,
										advance, italic_correction);
	}

	float opengl_surface_t::font_size(const unsigned int family) const
	{
		if (_font[family] != NULL) {
			return _font[family]->FaceSize();
		}
		else {
			return 0.0F;
		}
	}
#endif // HAVE_OPENGL

}
