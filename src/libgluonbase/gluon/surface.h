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

#ifndef GLUON_SURFACE_H_
#define GLUON_SURFACE_H_

#include <string>
#include <iostream>
#include <sstream>
#include <stdint.h>
#ifdef HAVE_SDL
#include <SDL/SDL.h>
#endif // HAVE_SDL
#ifdef HAVE_OPENGL
#include <GL/gl.h>
#ifdef HAVE_FTGL
#include <FTGL/FTGLBitmapFont.h>
#include <FTGL/FTGLPixmapFont.h>
#endif // HAVE_FTGL
#endif // HAVE_OPENGL

#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
#include <ft2build.h>
#include FT_FREETYPE_H
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)

#include <gluon/rect.h>
#include <gluon/color.h>
#include <mathtext/mathrender.h>
#include <gluon/plot2d.h>

namespace gluon {

    // An object oriented high-level interface to SDL

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 869)
#endif // __INTEL_COMPILER
	class surface_t
		: public mathtext::math_text_renderer_t, public plot_2d_t {
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
		enum {
			DISPLAY_LIST_UNSCISSORED = 0,
			DISPLAY_LIST_SCISSORED,
			NDISPLAY_LIST
		};
	protected:
		bool _master;
		surface_t *_supersurface;
		std::vector<surface_t *> _subsurface;
		float _font_size;
		color_t _current_color;
		float _current_alpha;
		float _current_font_size[NFAMILY];
		void delete_subsurface(void);
		bool delete_supersurface(void);
		inline virtual void begin_coordinate_matrix(void)
		{
		}
		inline virtual void end_coordinate_matrix(void)
		{
		}
		void
		text_alignment_transform(float &x, float &y,
								 const mathtext::bounding_box_t
								 bounding_box,
								 const unsigned int center,
								 const float distance = 0) const;
		inline virtual void begin_unscissored(void)
		{
			begin_coordinate_matrix();
		}
		void
		text_with_bounding_box(const float x, const float y,
							   const std::wstring string,
							   const unsigned int family =
							   FAMILY_PLAIN);
		void
		text_with_bounding_box(const float x, const float y,
							   const unsigned int center,
							   const std::wstring string,
							   const unsigned int family =
							   FAMILY_PLAIN);
		inline mathtext::affine_transform_t
		transform_logical_to_pixel(void) const
		{
			return _coordinate_matrix ? _transform_logical_to_pixel :
				affine_transform_t::identity;
		}
		inline mathtext::affine_transform_t
		transform_pixel_to_logical(void) const
		{
			return _coordinate_matrix ? _transform_pixel_to_logical :
				affine_transform_t::identity;
		}
	public:
		inline surface_t(const rect_t &rect, const bool master = false)
			: plot_2d_t(rect), _master(master), _supersurface(NULL)
		{
		}
		inline surface_t(surface_t &surface)
			: plot_2d_t(surface._rect), _master(false),
			  _supersurface(&surface)
		{
		}
		~surface_t(void);
		surface_t &surface(const rect_t &rect);
		surface_t &surface(const short x, const short y,
						   const unsigned short width,
						   const unsigned short height);
		inline rect_t rect(void) const
		{
			return _rect;
		}
		inline rect_t screen_rect(void) const
		{
			return _master ? _rect : _supersurface->screen_rect();
		}
		inline virtual void scissor(void) const
		{
		}
		inline virtual void unscissor(void) const
		{
		}
		inline virtual void begin(void)
		{
			begin_coordinate_matrix();
		}
		inline virtual void end(void)
		{
			end_coordinate_matrix();
		}
		inline virtual void update(void)
		{
		}
		/////////////////////////////////////////////////////////////
		// Color
		inline virtual void set_color(const color_t color)
		{
			_current_color = color;
		}
		inline virtual void set_lab_color(const float red,
										  const float green,
										  const float blue)
		{
			_current_color = color_t(color_t::COLOR_SPACE_LAB,
									 red, green, blue);
		}
		inline virtual void set_rgb_color(const float red,
										  const float green,
										  const float blue)
		{
			_current_color = color_t(color_t::COLOR_SPACE_RGB,
									 red, green, blue);
		}
		inline virtual void set_rgba_color(const float red,
										   const float green,
										   const float blue,
										   const float alpha)
		{
			_current_color = color_t(color_t::COLOR_SPACE_RGB,
									 red, green, blue);
			_current_alpha = alpha;
		}
		inline virtual void get_rgb_color(float &red, float &green,
										  float &blue) const
		{
			red = _current_color.red();
			green = _current_color.green();
			blue = _current_color.blue();
		}
		inline virtual void
		get_rgba_color(float &red, float &green, float &blue,
					   float &alpha) const
		{
			get_rgb_color(red, green, blue);
			alpha = _current_alpha;
		}
		color_t get_color(void) const;
		/////////////////////////////////////////////////////////////
		// Graphics primitives
		inline virtual void set_point_size(const float size)
		{
		}
		inline virtual void point(const float x, const float y)
		{
		}
		inline virtual void set_line_width(const float width)
		{
		}
		inline virtual void line(const float x0, const float y0,
								 const float x1, const float y1)
		{
		}
		inline virtual void
		rectangle(const float x, const float y, const float width,
				  const float height)
		{
		}
		inline virtual void
		rectangle(const mathtext::bounding_box_t &bounding_box)
		{
		}
		inline virtual void
		filled_rectangle(const float x, const float y,
						 const float width, const float height)
		{
		}
		inline virtual void
		filled_rectangle(const mathtext::bounding_box_t &
						 bounding_box)
		{
		}
		inline virtual void polygon(const polygon_t &p)
		{
		}
		inline virtual void filled_polygon(const polygon_t &p)
		{
		}
		inline virtual void
		raster(const float x, const float y, const float width,
			   const float height, const float density[],
			   const int nhorizontal, const int nvertical)
		{
		}
		/////////////////////////////////////////////////////////////
		// Typography
		inline virtual void
		open_font_overwrite(const std::string &filename,
							const unsigned int family = FAMILY_PLAIN)
		{
		}
		inline virtual void
		open_font_default(const std::string &filename,
						  const unsigned int family = FAMILY_PLAIN)
		{
		}
		void
		open_font(const std::string &filename_stix_regular,
				  const std::string &filename_stix_italic,
				  const std::string &filename_stix_bold,
				  const std::string &filename_stix_bold_italic);
		void
		open_font(const std::string &filename_stix_regular,
				  const std::string &filename_stix_italic,
				  const std::string &filename_stix_bold,
				  const std::string &filename_stix_bold_italic,
				  const std::string &filename_stix_size_1_regular,
				  const std::string &filename_stix_size_1_bold,
				  const std::string &filename_stix_size_2_regular,
				  const std::string &filename_stix_size_2_bold,
				  const std::string &filename_stix_size_3_regular,
				  const std::string &filename_stix_size_3_bold,
				  const std::string &filename_stix_size_4_regular,
				  const std::string &filename_stix_size_4_bold);
		void
		open_font(const std::string &filename_regular,
				  const std::string &filename_italic,
				  const std::string &filename_bold,
				  const std::string &filename_bold_italic,
				  const std::string &filename_stix_regular,
				  const std::string &filename_stix_italic,
				  const std::string &filename_stix_bold,
				  const std::string &filename_stix_bold_italic);
		void
		open_font(const std::string &filename_regular,
				  const std::string &filename_italic,
				  const std::string &filename_bold,
				  const std::string &filename_bold_italic,
				  const std::string &filename_stix_regular,
				  const std::string &filename_stix_italic,
				  const std::string &filename_stix_bold,
				  const std::string &filename_stix_bold_italic,
				  const std::string &filename_stix_size_1_regular,
				  const std::string &filename_stix_size_1_bold,
				  const std::string &filename_stix_size_2_regular,
				  const std::string &filename_stix_size_2_bold,
				  const std::string &filename_stix_size_3_regular,
				  const std::string &filename_stix_size_3_bold,
				  const std::string &filename_stix_size_4_regular,
				  const std::string &filename_stix_size_4_bold);
		void open_font(const std::string &basename);
		inline virtual void
		set_font_size(const float size, const unsigned int family)
		{
			_current_font_size[family] = size;
		}
		inline virtual void set_font_size(const float size)
		{
			_font_size = size;
			std::fill(_current_font_size,
					  _current_font_size + NFAMILY, size);

		}
		inline virtual void
		reset_font_size(const unsigned int family)
		{
			_current_font_size[family] = _font_size;
		}
		inline float font_size(void) const
		{
			return _font_size;
		}
		inline virtual mathtext::bounding_box_t
		bounding_box(const std::wstring string,
					 const unsigned int family)
		{
			return mathtext::bounding_box_t(0, 0, 0, 0, 0, 0);
		}
		inline virtual void
		text_raw(const float x, const float y,
				 const std::wstring string,
				 const unsigned int family)
		{
		}
		void text(const float x, const float y,
				  const std::wstring string,
				  const unsigned int family = FAMILY_PLAIN);
		inline virtual float
		font_size(const unsigned int family = FAMILY_PLAIN) const
		{
			return _current_font_size[family];
		}
		/////////////////////////////////////////////////////////////
		virtual void text(const float x, const float y,
						  const unsigned int alignment,
						  const mathtext::math_text_t &math_text,
						  const float distance = 0.0F);
		using math_text_renderer_t::bounding_box;
		using math_text_renderer_t::text_raw;
	};
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

#if defined(HAVE_SDL) && defined(HAVE_OPENGL)
	class opengl_surface_t : public surface_t {
	protected:
		int16_t _depth;
		GLuint _display_list;
		FTFont *_font[NFAMILY];
	protected:
		void generate_display_list(void);
		void delete_display_list(void);
		void delete_subsurface(void);
		bool delete_supersurface(void);
		void begin_coordinate_matrix(void);
		inline void end_coordinate_matrix(void)
		{
			glPopMatrix();
		}
		void
		text_alignment_transform(float &x, float &y,
								 const mathtext::bounding_box_t
								 bounding_box,
								 const unsigned int center,
								 const float distance = 0) const;
		void begin_unscissored(void);
	public:
		opengl_surface_t(const rect_t &rect,
						 const bool master = true,
						 const int16_t depth = 32);
		opengl_surface_t(opengl_surface_t &surface);
		virtual ~opengl_surface_t(void);
		opengl_surface_t &surface(const rect_t &rect);
		opengl_surface_t &surface(const short x, const short y,
								  const unsigned short width,
								  const unsigned short height);
		/////////////////////////////////////////////////////////////
		// Graphics state
		void scissor(void) const;
		void unscissor(void) const;
		void begin(void);
		void end(void);
		void update(void);
		/////////////////////////////////////////////////////////////
		// Color
		inline void set_color(const color_t color)
		{
			glColor3f(color.red(), color.green(), color.blue());
		}
		inline void set_rgb_color(const float red,
								  const float green,
								  const float blue)
		{
			glColor3f(red, green, blue);
		}
		inline void set_rgba_color(const float red,
								   const float green,
								   const float blue,
								   const float alpha)
		{
			glColor4f(red, green, blue, alpha);
		}
		void get_rgb_color(float &red, float &green,
						   float &blue) const;
		void get_rgba_color(float &red, float &green,
							float &blue, float &alpha) const;
		color_t get_color(void) const;
		/////////////////////////////////////////////////////////////
		// Graphics primitives
		inline void set_point_size(const float size)
		{
			glPointSize(size);
		}
		void point(const float x, const float y) const;
		inline void set_line_width(const float width)
		{
			glLineWidth(width);
		}
		void line(const float x0, const float y0,
				  const float x1, const float y1) const;
		void rectangle(const float x, const float y,
					   const float width, const float height) const;
		void rectangle(const mathtext::bounding_box_t &bounding_box)
			const;
		void filled_rectangle(const float x, const float y,
							  const float width, const float height)
			const;
		void filled_rectangle(const mathtext::bounding_box_t &
							  bounding_box) const;
		void polygon(const polygon_t &p) const;
		void filled_polygon(const polygon_t &p) const;
		void raster(const float x, const float y,
					const float width, const float height,
					const float density[],
					const int nhorizontal, const int nvertical) const;
		/////////////////////////////////////////////////////////////
		// Typography
		void open_font_overwrite(const std::string &filename,
								 const unsigned int family =
								 FAMILY_PLAIN);
		void open_font_default(const std::string &filename,
							   const unsigned int family =
							   FAMILY_PLAIN);
		void set_font_size(const float size,
						   const unsigned int family);
		void set_font_size(const float size);
		void reset_font_size(const unsigned int family);
		mathtext::bounding_box_t
		bounding_box(const std::wstring string,
					 const unsigned int family = FAMILY_PLAIN);
		void
		text_raw(const float x, const float y,
				 const std::wstring string,
				 const unsigned int family = FAMILY_PLAIN);
		float
		font_size(const unsigned int family = FAMILY_PLAIN) const;
		/////////////////////////////////////////////////////////////
		using surface_t::set_color;
		using surface_t::set_rgb_color;
		using surface_t::set_rgba_color;
		using surface_t::font_size;
	};
#endif // defined(HAVE_SDL) && defined(HAVE_OPENGL)

	class postscript_surface_t : public surface_t {
	private:
		void open_output_file(void);
		void write_dsc_preamble(void) const;
		void write_dsc_trailer(void) const;
		void close_output_file(void);
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		void freetype_init(void);
		void read_font_data(std::vector<FT_Byte> &font_data,
							const std::string &filename) const;
		FT_Glyph_Metrics &
		freetype_metrics(const wchar_t character,
						 const unsigned int family) const;
		bool
		parse_otf_cff_header(std::string &font_name,
							 unsigned int &cff_offset,
							 unsigned int &cff_length,
							 const std::vector<FT_Byte> font_data)
			const;
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		unsigned int ascii85_line_count(const uint8_t *buffer,
										const size_t length) const;
		void
		append_ascii85(std::string &ascii, const uint8_t *buffer,
					   const size_t length) const;
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		std::string
		font_embed_type_2(std::string &font_name,
						  const std::vector<FT_Byte> &font_data)
			const;
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
	protected:
		std::string _filename;
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		FT_Library _freetype_library;
		FT_Face _font[NFAMILY];
		std::vector<FT_Byte> _font_data[NFAMILY];
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		std::string _font_name[NFAMILY];
		FILE *_fp;
		float _current_point_size;
	public:
		postscript_surface_t(const rect_t &rect, const bool master);
		postscript_surface_t(const std::string &filename,
							 const uint16_t width,
							 const uint16_t height);
		postscript_surface_t(postscript_surface_t &surface);
		~postscript_surface_t(void);
		postscript_surface_t &surface(const rect_t &rect);
		postscript_surface_t &surface(const short x, const short y,
									  const unsigned short width,
									  const unsigned short height);
		/////////////////////////////////////////////////////////////
		// Color
		void set_color(const color_t color);
		void set_rgb_color(const float red, const float green,
						   const float blue);
		void set_rgba_color(const float red, const float green,
							const float blue, const float alpha);
		/////////////////////////////////////////////////////////////
		// Graphics primitives
		void set_point_size(const float size);
		void point(const float x, const float y) const;
		void set_line_width(const float width);
		void line(const float x0, const float y0,
				  const float x1, const float y1) const;
		void rectangle(const float x, const float y,
					   const float width, const float height) const;
		void rectangle(const mathtext::bounding_box_t &bounding_box)
			const;
		void
		filled_rectangle(const float x, const float y,
						 const float width, const float height) const;
		void
		filled_rectangle(const mathtext::bounding_box_t &
						 bounding_box) const;
		void
		raster(const float x, const float y, const float width,
			   const float height, const float density[],
			   const int nhorizontal, const int nvertical) const;
		/////////////////////////////////////////////////////////////
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		void open_font_overwrite(const std::string &filename,
								 const unsigned int family =
								 FAMILY_PLAIN);
		void open_font_default(const std::string &filename,
							   const unsigned int family =
							   FAMILY_PLAIN);
		void text_raw(const float x, const float y,
					  const std::wstring string,
					  const unsigned int family);
		mathtext::bounding_box_t
		bounding_box(const std::wstring string,
					 const unsigned int family);
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
	};

}

#endif // GLUON_SURFACE_H_
