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
#include <endian.h>
#include <byteswap.h>
#include <sys/time.h>
#include <gluon/screen.h>
#include <mathtext/fontembed.h>

/////////////////////////////////////////////////////////////////////

namespace gluon {

	void postscript_surface_t::open_output_file(void)
	{
		_fp = fopen(_filename.c_str(), "w");
	}

	void postscript_surface_t::write_dsc_preamble(void) const
	{
		static const std::string creator = "libgluon";

		fputs("%!PS-Adobe-3.0 EPSF-3.0\n", _fp);
		fprintf(_fp, "%%%%BoundingBox: %.0f %.0f %.0f %.0f\n",
				floor(_rect.left()),
				floor(_rect.bottom()),
				ceil(_rect.right()),
				ceil(_rect.top()));
		fprintf(_fp, "%%%%HiResBoundingBox: %.3f %.3f %.3f %.3f\n",
				_rect.left(), _rect.bottom(),
				_rect.right(), _rect.top());
		fprintf(_fp, "%%%%Creator: %s\n", creator.c_str());
		fprintf(_fp, "%%%%Title: (%s)\n", _filename.c_str());

		time_t creation_time = time(NULL);

		fprintf(_fp, "%%%%CreationDate: %s",
				asctime(localtime(&creation_time)));
		fputs("%%LanguageLevel: 3\n", _fp);
		fputs("%%EndComments\n", _fp);
	}

	void postscript_surface_t::write_dsc_trailer(void) const
	{
		fputs("%%EOF\n", _fp);
	}

	void postscript_surface_t::close_output_file(void)
	{
		if (_fp != NULL) {
			fclose(_fp);
			_fp = NULL;
		}
	}

#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
	void postscript_surface_t::freetype_init(void)
	{
		const FT_Error error = FT_Init_FreeType(&_freetype_library);

		if (error != 0) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: FreeType initialization error"
					  << std::endl;
		}
	}

	FT_Glyph_Metrics &postscript_surface_t::
	freetype_metrics(const wchar_t character,
					 const unsigned int family) const
	{
		FT_Load_Glyph(_font[family],
					  FT_Get_Char_Index(_font[family],
										character),
					  FT_LOAD_NO_SCALE);

		return _font[family]->glyph->metrics;
	}

	void postscript_surface_t::
	read_font_data(std::vector<FT_Byte> &font_data,
				   const std::string &filename)
		const
	{
		FILE *fp = fopen(filename.c_str(), "r");

		if (fp == NULL)
			return;
		if (fseek(fp, 0L, SEEK_END) == -1) {
			perror("fseek");
			return;
		}

		const long length = ftell(fp);

		if (length == -1) {
			perror("ftell");
			return;
		}
		font_data.resize(length);
		if (fseek(fp, 0L, SEEK_SET) == -1) {
			perror("fseek");
			return;
		}
		if (fread(&font_data[0], sizeof(FT_Byte), length, fp) !=
		   static_cast<size_t>(length)) {
			perror("fread");
			return;
		}
	}

	void postscript_surface_t::
	open_font_overwrite(const std::string &filename,
						const unsigned int family)
	{
		if (_font[family] != NULL)
			FT_Done_Face(_font[family]);
		read_font_data(_font_data[family], filename);

		const FT_Error error =
			FT_New_Memory_Face(_freetype_library,
							   &_font_data[family][0],
							   _font_data[family].size(), 0,
							   &_font[family]);
		if (error != 0) {
			return;
		}

		std::string embedded_str;

		if (_fp != NULL) {
			 embedded_str =
				 mathtext::font_embed_postscript_t::
				 font_embed_type_2(_font_name[family],
								   _font_data[family]);
		}

		if (_master) {
			bool embed_success = false;

			if (!embedded_str.empty()) {
				fputs(embedded_str.c_str(), _fp);
				embed_success = true;
			}
			if (!embed_success) {
				std::cerr << __FILE__ << ':' << __LINE__
						  << ": error: could not embed font `"
						  << filename << '\'' << std::endl;
			}
		}
	}

	void postscript_surface_t::
	open_font_default(const std::string &filename,
					  const unsigned int family)
	{
		if (_font[family] == NULL)
			open_font_overwrite(filename, family);
	}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)

	postscript_surface_t::
	postscript_surface_t(const rect_t &rect, const bool master)
		: surface_t(rect, master), _fp(NULL), _current_point_size(0)
	{
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		freetype_init();
		std::fill(_font, _font + NFAMILY,
				  reinterpret_cast<FT_Face>(NULL));
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		if (master) {
			open_output_file();
			write_dsc_preamble();
		}
	}

	postscript_surface_t::
	postscript_surface_t(const std::string &filename,
						 const uint16_t width, const uint16_t height)
		: surface_t(rect_t(0, 0, width, height), true),
		  _filename(filename), _fp(NULL), _current_point_size(0)
	{
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		freetype_init();
		std::fill(_font, _font + NFAMILY,
				  reinterpret_cast<FT_Face>(NULL));
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		open_output_file();
		write_dsc_preamble();
	}

	postscript_surface_t::
	postscript_surface_t(postscript_surface_t &surface)
		: surface_t(surface)
	{
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		freetype_init();
		std::fill(_font, _font + NFAMILY,
				  reinterpret_cast<FT_Face>(NULL));
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		surface._subsurface.push_back(this);
		_fp = surface._fp;
	}

	postscript_surface_t::~postscript_surface_t(void)
	{
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		for (unsigned int family = 0; family < NFAMILY; family++)
			if (_font[family] != NULL) {
				FT_Done_Face(_font[family]);
				_font[family] = NULL;
			}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		if (_master) {
			write_dsc_trailer();
			close_output_file();
		}
	}

	postscript_surface_t &postscript_surface_t::
	surface(const rect_t &rect)
	{
		const rect_t scissored_rect = _rect.scissor(rect);
		postscript_surface_t *ret =
			new postscript_surface_t(scissored_rect, false);

		_subsurface.push_back(ret);
		ret->_supersurface = this;
		ret->_fp = _fp;
#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		ret->_freetype_library = _freetype_library;
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)

		return *ret;
	}

	postscript_surface_t &postscript_surface_t::
	surface(const short x, const short y,
			const unsigned short width,
			const unsigned short height)
	{
		return surface(rect_t(x, y, width, height));
	}

	void postscript_surface_t::set_color(const color_t color)
	{
		_current_color = color;

		fprintf(_fp, "/DeviceRGB setcolorspace "
				"%.3f %.3f %.3f setcolor\n",
				color.red(), color.green(), color.blue());
	}

	void postscript_surface_t::set_rgb_color(const float red,
											 const float green,
											 const float blue)
	{
		set_color(color_t(color_t::COLOR_SPACE_RGB,
						  red, green, blue));
	}

	void postscript_surface_t::set_rgba_color(const float red,
											  const float green,
											  const float blue,
											  const float alpha)
	{
		set_color(color_t(color_t::COLOR_SPACE_RGB,
						  red, green, blue));
		_current_alpha = alpha;
	}

	void postscript_surface_t::
	set_point_size(const float size)
	{
		_current_point_size = size;
	}

	void postscript_surface_t::
	point(const float x, const float y) const
	{
		if (_current_point_size != 0) {
			const point_t transformed = _transform_logical_to_pixel *
				point_t(x, y);

			fprintf(_fp, "newpath %.3f %.3f %.3f 0 360 arc "
					"closepath fill\n",
					transformed[0],
					screen_rect().bottom() - transformed[1],
					0.5F * _current_point_size);
		}
	}

	void postscript_surface_t::
	set_line_width(const float width)
	{
		fprintf(_fp, "%.3f setlinewidth\n", width);
	}

	void postscript_surface_t::
	line(const float x0, const float y0,
		 const float x1, const float y1) const
	{
		const point_t transformed0 = _transform_logical_to_pixel *
			point_t(x0, y0);
		const point_t transformed1 = _transform_logical_to_pixel *
			point_t(x1, y1);

		fprintf(_fp, "newpath %.3f %.3f moveto %.3f %.3f lineto "
				"stroke\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed1[1]);
	}

	void postscript_surface_t::
	rectangle(const float x, const float y,
			  const float width, const float height) const
	{
		const point_t transformed0 = _transform_logical_to_pixel *
			point_t(x, y);
		const point_t transformed1 = _transform_logical_to_pixel *
			point_t(x + width, y + height);

		fprintf(_fp, "newpath %.3f %.3f moveto %.3f %.3f lineto "
				"%.3f %.3f moveto %.3f %.3f lineto "
				"closepath stroke\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed1[1],
				transformed0[0],
				screen_rect().bottom() - transformed1[1]);
	}

	void postscript_surface_t::
	rectangle(const mathtext::bounding_box_t &bounding_box) const
	{
		const point_t transformed0 = _transform_logical_to_pixel *
			bounding_box.lower_left();
		const point_t transformed1 = _transform_logical_to_pixel *
			bounding_box.upper_right();

		fprintf(_fp, "newpath %.3f %.3f moveto %.3f %.3f lineto "
				"%.3f %.3f moveto %.3f %.3f lineto "
				"closepath stroke\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed1[1],
				transformed0[0],
				screen_rect().bottom() - transformed1[1]);
	}

	void postscript_surface_t::
	filled_rectangle(const float x, const float y,
					 const float width, const float height) const
	{
		const point_t transformed0 = _transform_logical_to_pixel *
			point_t(x, y);
		const point_t transformed1 = _transform_logical_to_pixel *
			point_t(x + width, y + height);

		fprintf(_fp, "newpath %.3f %.3f moveto %.3f %.3f lineto "
				"%.3f %.3f moveto %.3f %.3f lineto "
				"closepath fill\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed1[1],
				transformed0[0],
				screen_rect().bottom() - transformed1[1]);
	}

	void postscript_surface_t::
	filled_rectangle(const mathtext::bounding_box_t &bounding_box)
		const
	{
		const point_t transformed0 = _transform_logical_to_pixel *
			bounding_box.lower_left();
		const point_t transformed1 = _transform_logical_to_pixel *
			bounding_box.upper_right();

		fprintf(_fp, "newpath %.3f %.3f moveto %.3f %.3f lineto\n"
				"%.3f %.3f lineto %.3f %.3f lineto "
				"closepath fill\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0],
				screen_rect().bottom() - transformed1[1],
				transformed0[0],
				screen_rect().bottom() - transformed1[1]);
	}

	void postscript_surface_t::
	raster(const float x, const float y, const float width,
		   const float height, const float density[],
		   const int nhorizontal, const int nvertical) const
	{
		fputs("/DeviceRGB setcolorspace\n", _fp);

		const point_t transformed0 = _transform_logical_to_pixel *
			point_t(x, y);
		const point_t transformed1 = _transform_logical_to_pixel *
			point_t(x + width, y + height);
		fprintf(_fp, "gsave %.3f %.3f translate %.3f %.3f scale\n",
				transformed0[0],
				screen_rect().bottom() - transformed0[1],
				transformed1[0] - transformed0[0],
				transformed1[1] - transformed0[1]);
		fputs("<<\n", _fp);
		fputs("    /ImageType 1\n", _fp);
		fprintf(_fp, "    /Width %d\n", nhorizontal);
		fprintf(_fp, "    /Height %d\n", nvertical);
		fputs("    /BitsPerComponent 8\n", _fp);
		fputs("    /Decode [0 1 0 1 0 1]\n", _fp);
		fprintf(_fp, "    /ImageMatrix [%d 0 0 %d 0 0]\n",
				nhorizontal, nvertical);
		fputs("    /DataSource currentfile /ASCII85Decode filter\n",
			  _fp);
		fputs(">>\n", _fp);
		fputs("image\n", _fp);

		const unsigned long npixel = nhorizontal * nvertical;
		uint8_t *buffer = new uint8_t[npixel * 3];

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 810)
#endif // __INTEL_COMPILER
		for (unsigned long i = 0; i < npixel; i++)
			for (unsigned long j = 0; j < 3; j++)
				buffer[i * 3 + j] = density[(i << 2) + j] < 0 ? 0U :
					density[(i << 2) + j] < (255.0 / 256.0) ?
					static_cast<uint8_t>(
						density[(i << 2) + j] * 256.0F) : 255U;
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

		std::string ascii;

		append_ascii85(ascii, reinterpret_cast<uint8_t *>(buffer),
					   npixel * 3);

		delete [] buffer;

		fputs(ascii.c_str(), _fp);
		fputs("\n", _fp);
		fputs("grestore\n", _fp);
	}

#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
	mathtext::bounding_box_t postscript_surface_t::
	bounding_box(const std::wstring string, const unsigned int family)
	{
		if (string.empty() || _font[family] == NULL ||
		   _font[family]->units_per_EM == 0)
			return mathtext::bounding_box_t(0, 0, 0, 0, 0, 0);

		std::wstring::const_iterator iterator = string.begin();

		const float scale = _current_font_size[family] /
			_font[family]->units_per_EM;
		FT_Glyph_Metrics metrics;
		mathtext::bounding_box_t ret;

		metrics = freetype_metrics(*iterator, family);
		ret = mathtext::bounding_box_t(
			metrics.horiBearingX * scale,
			(-metrics.horiBearingY) * scale,
			(metrics.horiBearingX + metrics.width) * scale,
			(-metrics.horiBearingY + metrics.height) * scale,
			metrics.horiAdvance * scale, 0);
		iterator++;

		float current_x = metrics.horiAdvance;

		for (; iterator != string.end(); iterator++) {
			metrics = freetype_metrics(*iterator, family);
			const mathtext::bounding_box_t glyph_bounding_box =
				mathtext::point_t(0, current_x) +
				mathtext::bounding_box_t(
					metrics.horiBearingX * scale,
					(-metrics.horiBearingY) * scale,
					(metrics.horiBearingX + metrics.width) * scale,
					(-metrics.horiBearingY + metrics.height) * scale,
					metrics.horiAdvance * scale, 0);
			ret = ret.merge(glyph_bounding_box);
			current_x += metrics.horiAdvance;
		}

		return ret;
	}

	void postscript_surface_t::
	text_raw(const float x, const float y, const std::wstring string,
			 const unsigned int family)
	{
		if (family >= NFAMILY || _font[family] == NULL) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: font not initialized"
					  << std::endl;
			return;
		}
		if (_current_font_size[family] <= 0)
			return;
		// FIXME: Check if reloading/rescaling the font is necessary

		point_t transformed = _transform_logical_to_pixel * point_t(x, y);

		fprintf(_fp, "/%s findfont %.3f scalefont setfont\n"
				"%.3f %.3f moveto ", _font_name[family].c_str(),
				_current_font_size[family], transformed[0],
				screen_rect().bottom() - transformed[1]);

#include <table/adobeglyph.h>

		for (std::wstring::const_reverse_iterator iterator =
				string.rbegin();
			iterator != string.rend(); iterator++) {
			const wchar_t *lower =
				std::lower_bound(adobe_glyph_ucs,
								 adobe_glyph_ucs + nadobe_glyph,
								 *iterator);
			if (lower < adobe_glyph_ucs + nadobe_glyph &&
			   *lower == *iterator) {
				const unsigned long index =
					lower - adobe_glyph_ucs;

				fprintf(_fp, "/%s ", adobe_glyph_name[index]);
			}
			else
				fprintf(_fp, "/uni%04X ", *iterator);
		}
		// FIXME: extract 8-bit segment and use "show"
		if (string.size() == 1U)
			fprintf(_fp, "glyphshow\n");
		else
			fprintf(_fp, "%lu {glyphshow} repeat\n",
					string.size());
	}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
}
