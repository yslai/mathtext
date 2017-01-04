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

	void surface_t::delete_subsurface(void)
	{
		for(std::vector<surface_t *>::iterator iterator =
				_subsurface.begin();
			iterator != _subsurface.end(); iterator++)
			if(*iterator != NULL) {
				delete *iterator;
				*iterator = NULL;
			}
	}

	bool surface_t::delete_supersurface(void)
	{
		for(std::vector<surface_t *>::iterator iterator =
				_supersurface->_subsurface.begin();
			iterator != _supersurface->_subsurface.end();
			iterator++)
			if(*iterator == this) {
				*iterator = NULL;
				return true;
			}

		return false;
	}

	void surface_t::
	text_alignment_transform(float &x, float &y,
							 const mathtext::bounding_box_t
							 bounding_box,
							 const unsigned int alignment,
							 const float distance) const
	{
		mathtext::point_t displacement(0, 0);

		switch(alignment) {
		case ALIGN_LOWER_LEFT:
			displacement[0] -= bounding_box.left();
			displacement[1] -= bounding_box.bottom();
			displacement[0] += (float)M_SQRT1_2 * distance;
			displacement[1] += (float)M_SQRT1_2 * distance;
			break;
		case ALIGN_LOWER_CENTER:
			displacement[0] -= bounding_box.horizontal_center();
			displacement[1] -= bounding_box.bottom();
			displacement[1] += distance;
			break;
		case ALIGN_LOWER_RIGHT:
			displacement[0] -= bounding_box.right();
			displacement[1] -= bounding_box.bottom();
			displacement[0] -= (float)M_SQRT1_2 * distance;
			displacement[1] += (float)M_SQRT1_2 * distance;
			break;
		case ALIGN_CENTER_RIGHT:
			displacement[0] -= bounding_box.right();
			displacement[1] -= bounding_box.vertical_center();
			displacement[0] -= distance;
			break;
		case ALIGN_UPPER_RIGHT:
			displacement[0] -= bounding_box.right();
			displacement[1] -= bounding_box.top();
			displacement[0] -= (float)M_SQRT1_2 * distance;
			displacement[1] += (float)M_SQRT1_2 * distance;
			break;
		case ALIGN_UPPER_CENTER:
			displacement[0] -= bounding_box.horizontal_center();
			displacement[1] -= bounding_box.top();
			displacement[1] -= distance;
			break;
		case ALIGN_UPPER_LEFT:
			displacement[0] -= bounding_box.left();
			displacement[1] -= bounding_box.top();
			displacement[0] += (float)M_SQRT1_2 * distance;
			displacement[1] -= (float)M_SQRT1_2 * distance;
			break;
		case ALIGN_CENTER_LEFT:
			displacement[0] -= bounding_box.left();
			displacement[1] -= bounding_box.vertical_center();
			displacement[0] += distance;
			break;
		case ALIGN_CENTER:
			displacement[0] -= bounding_box.horizontal_center();
			displacement[1] -= bounding_box.vertical_center();
			break;
		}

		const mathtext::point_t displacement_transformed =
			transform_pixel_to_logical().linear() * displacement;

		x += displacement_transformed[0];
		y += displacement_transformed[1];
	}

	surface_t::~surface_t(void)
	{
		delete_subsurface();
		if(!_master)
			delete_supersurface();
	}

	surface_t &surface_t::surface(const rect_t &rect)
	{
		const rect_t scissored_rect = _rect.scissor(rect);
		surface_t *ret = new surface_t(scissored_rect, false);

		_subsurface.push_back(ret);
		ret->_supersurface = this;

		return *ret;
	}

	surface_t &surface_t::surface(const short x, const short y,
								  const unsigned short width,
								  const unsigned short height)
	{
		return surface(rect_t(x, y, width, height));
	}

	void surface_t::
	text_with_bounding_box(const float x, const float y,
						   const std::wstring string,
						   const unsigned int family)
	{
		point(x, y);

		mathtext::bounding_box_t string_bounding_box =
			bounding_box(string, family);

		rectangle(mathtext::point_t(x, y) + string_bounding_box);
		if(string_bounding_box.descent() > 0)
			line(x + string_bounding_box.left(), y,
				 x + string_bounding_box.right(), y);
		if (string_bounding_box.italic_correction() > 0 &&
			string_bounding_box.ascent() != 0) {
			line(x + string_bounding_box.advance() +
				 string_bounding_box.italic_correction(),
				 y + string_bounding_box.ascent(),
				 x + string_bounding_box.advance() -
				 (string_bounding_box.italic_correction() /
				  string_bounding_box.ascent() *
				  string_bounding_box.descent()),
				 y - string_bounding_box.descent());
		}

		text(x, y, string, family);
	}

	color_t surface_t::get_color(void) const
	{
		float red;
		float green;
		float blue;

		get_rgb_color(red, green, blue);

		return color_t(color_t::COLOR_SPACE_RGB, red, green, blue);
	}

	/////////////////////////////////////////////////////////////////

	void surface_t::
	open_font(const std::string &filename_stix_regular,
			  const std::string &filename_stix_italic,
			  const std::string &filename_stix_bold,
			  const std::string &filename_stix_bold_italic)
	{
		open_font_default(filename_stix_regular, FAMILY_PLAIN);
		open_font_overwrite(filename_stix_regular, FAMILY_REGULAR);
		open_font_overwrite(filename_stix_italic, FAMILY_ITALIC);
		open_font_overwrite(filename_stix_bold, FAMILY_BOLD);
		open_font_overwrite(filename_stix_bold_italic,
							FAMILY_BOLD_ITALIC);
		open_font_overwrite(filename_stix_regular,
							FAMILY_STIX_REGULAR);
		open_font_overwrite(filename_stix_italic,
							FAMILY_STIX_ITALIC);
		open_font_overwrite(filename_stix_bold, FAMILY_STIX_BOLD);
		open_font_overwrite(filename_stix_bold_italic,
							FAMILY_STIX_BOLD_ITALIC);
	}

	void surface_t::
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
			  const std::string &filename_stix_size_4_bold)
	{
		open_font(filename_stix_regular, filename_stix_italic,
				  filename_stix_bold, filename_stix_bold_italic);
		open_font_overwrite(filename_stix_size_1_regular,
				  FAMILY_STIX_SIZE_1_REGULAR);
		open_font_overwrite(filename_stix_size_1_bold,
				  FAMILY_STIX_SIZE_1_BOLD);
		open_font_overwrite(filename_stix_size_2_regular,
				  FAMILY_STIX_SIZE_2_REGULAR);
		open_font_overwrite(filename_stix_size_2_bold,
				  FAMILY_STIX_SIZE_2_BOLD);
		open_font_overwrite(filename_stix_size_3_regular,
				  FAMILY_STIX_SIZE_3_REGULAR);
		open_font_overwrite(filename_stix_size_3_bold,
				  FAMILY_STIX_SIZE_3_BOLD);
		open_font_overwrite(filename_stix_size_4_regular,
				  FAMILY_STIX_SIZE_4_REGULAR);
		open_font_overwrite(filename_stix_size_4_bold,
				  FAMILY_STIX_SIZE_4_BOLD);
	}

	void surface_t::
	open_font(const std::string &filename_regular,
			  const std::string &filename_italic,
			  const std::string &filename_bold,
			  const std::string &filename_bold_italic,
			  const std::string &filename_stix_regular,
			  const std::string &filename_stix_italic,
			  const std::string &filename_stix_bold,
			  const std::string &filename_stix_bold_italic)
	{
		open_font_default(filename_regular, FAMILY_PLAIN);
		open_font_overwrite(filename_regular, FAMILY_REGULAR);
		open_font_overwrite(filename_italic, FAMILY_ITALIC);
		open_font_overwrite(filename_bold, FAMILY_BOLD);
		open_font_overwrite(filename_bold_italic,
							FAMILY_BOLD_ITALIC);
		open_font_overwrite(filename_stix_regular,
							FAMILY_STIX_REGULAR);
		open_font_overwrite(filename_stix_italic,
							FAMILY_STIX_ITALIC);
		open_font_overwrite(filename_stix_bold, FAMILY_STIX_BOLD);
		open_font_overwrite(filename_stix_bold_italic,
							FAMILY_STIX_BOLD_ITALIC);
	}

	void surface_t::
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
			  const std::string &filename_stix_size_4_bold)
	{
		open_font(filename_regular, filename_italic, filename_bold,
				  filename_bold_italic, filename_stix_regular,
				  filename_stix_italic, filename_stix_bold,
				  filename_stix_bold_italic);
		open_font_overwrite(filename_stix_size_1_regular,
							FAMILY_STIX_SIZE_1_REGULAR);
		open_font_overwrite(filename_stix_size_1_bold,
							FAMILY_STIX_SIZE_1_BOLD);
		open_font_overwrite(filename_stix_size_2_regular,
							FAMILY_STIX_SIZE_2_REGULAR);
		open_font_overwrite(filename_stix_size_2_bold,
							FAMILY_STIX_SIZE_2_BOLD);
		open_font_overwrite(filename_stix_size_3_regular,
							FAMILY_STIX_SIZE_3_REGULAR);
		open_font_overwrite(filename_stix_size_3_bold,
							FAMILY_STIX_SIZE_3_BOLD);
		open_font_overwrite(filename_stix_size_4_regular,
							FAMILY_STIX_SIZE_4_REGULAR);
		open_font_overwrite(filename_stix_size_4_bold,
							FAMILY_STIX_SIZE_4_BOLD);
	}

	void surface_t::open_font(const std::string &basename)
	{
		const unsigned long position = basename.rfind('/');
		std::string dirname = position == basename.size() ? "" :
			basename.substr(0, position + 1);

		open_font(basename + "-Regular.ttf",
				  basename + "-It.ttf",
				  basename + "-Bold.ttf",
				  basename + "-BoldIt.ttf",
				  dirname + "STIXGeneral-Regular.otf",
				  dirname + "STIXGeneral-Italic.otf",
				  dirname + "STIXGeneral-Bold.otf",
				  dirname + "STIXGeneral-BoldItalic.otf",
				  dirname + "STIXSizeOneSym-Regular.otf",
				  dirname + "STIXSizeOneSym-Bold.otf",
				  dirname + "STIXSizeTwoSym-Regular.otf",
				  dirname + "STIXSizeTwoSym-Bold.otf",
				  dirname + "STIXSizeThreeSym-Regular.otf",
				  dirname + "STIXSizeThreeSym-Bold.otf",
				  dirname + "STIXSizeFourSym-Regular.otf",
				  dirname + "STIXSizeFourSym-Bold.otf");
	}

	void surface_t::
	text(const float x, const float y, const std::wstring string,
		 const unsigned int family)
	{
		text_raw(x, y, string, family);
	}

	void surface_t::
	text(const float x, const float y, const unsigned int alignment,
		 const mathtext::math_text_t &math_text,
		 const float distance)
	{
		const mathtext::bounding_box_t math_text_bounding_box =
			bounding_box(math_text);
		float x_transformed = x;
		float y_transformed = y;

		text_alignment_transform(x_transformed, y_transformed,
								 math_text_bounding_box, alignment,
								 distance);
		math_text_renderer_t::text(x_transformed, y_transformed,
								   math_text);
	}

}
