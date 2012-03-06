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
#include <mathtext/mathrender.h>

/////////////////////////////////////////////////////////////////////

namespace mathtext {

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 869)
#endif // __INTEL_COMPILER

	bounding_box_t math_text_renderer_t::
	math_bounding_box(const math_text_t::box_t &box,
					  const unsigned int style)
	{
		set_font_size(style_size(style), FAMILY_REGULAR);

		const bounding_box_t box_bounding_box =
			bounding_box(box._string, FAMILY_REGULAR);

		reset_font_size(FAMILY_REGULAR);

		return box_bounding_box;
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const math_text_t::box_t &box,
			  const unsigned int style,
			  const bool render_structure)
	{
		set_font_size(style_size(style), FAMILY_REGULAR);
		text_raw(origin[0], origin[1], box._string, FAMILY_REGULAR);
		reset_font_size(FAMILY_REGULAR);
	}
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

	bounding_box_t math_text_renderer_t::
	math_bounding_box(const wchar_t &glyph,
					  const unsigned int family, const float size)
	{
		set_font_size(size, family);

		const std::wstring string = std::wstring(1, glyph);
		const bounding_box_t math_symbol_bounding_box =
			bounding_box(string, family);

		reset_font_size(family);

		return math_symbol_bounding_box;
	}

	void math_text_renderer_t::
	math_text(const point_t origin, const wchar_t &glyph,
			  const unsigned int family, const float size,
			  const bool render_structure)
	{
		set_font_size(size, family);

		const std::wstring string = std::wstring(1, glyph);

		if(render_structure)
			text_with_bounding_box(origin[0], origin[1], string,
								   family);
		else
			text_raw(origin[0], origin[1], string, family);
		reset_font_size(family);
	}

	bounding_box_t math_text_renderer_t::
	math_bounding_box(const math_text_t::math_symbol_t &math_symbol,
					  const unsigned int style)
	{
		const unsigned int family = math_family(math_symbol);
		const float size = style_size(style);

		return math_bounding_box(math_symbol._glyph, family, size);
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const math_text_t::math_symbol_t &math_symbol,
			  const unsigned int style,
			  const bool render_structure)
	{
		const unsigned int family = math_family(math_symbol);
		const float size = style_size(style);

		math_text(origin, math_symbol._glyph, family, size,
				  render_structure);
	}

	bounding_box_t math_text_renderer_t::
	math_bounding_box(const math_text_t::math_symbol_t &math_symbol,
					  const unsigned int style, const float height)
	{
		std::vector<math_token_t> token =
			math_tokenize(math_symbol, style, height);

		if(token.empty())
			return bounding_box_t(0, 0, 0, 0, 0, 0);

		std::vector<math_token_t>::const_iterator iterator =
			token.begin();
		bounding_box_t ret = iterator->_offset +
			iterator->_bounding_box;

		for(; iterator != token.end(); iterator++)
			ret = ret.merge(iterator->_offset +
							iterator->_bounding_box);

		return ret;
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const math_text_t::math_symbol_t &math_symbol,
			  const unsigned int style, const float height,
			  const bool render_structure)
	{
		std::vector<math_token_t> token =
			math_tokenize(math_symbol, style, height);

		for(std::vector<math_token_t>::const_iterator iterator =
				token.begin();
			iterator != token.end(); iterator++)
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  iterator->_offset,
					  iterator->_extensible._glyph,
					  iterator->_extensible._family,
					  iterator->_extensible._size, render_structure);
	}

	/////////////////////////////////////////////////////////////////

	// A field can be a math symbol or a math list
	bounding_box_t math_text_renderer_t::
	math_bounding_box(const std::vector<math_text_t::item_t>::
					  const_iterator &math_list_begin,
					  const std::vector<math_text_t::item_t>::
					  const_iterator &math_list_end,
					  unsigned int style)
	{
		std::vector<math_token_t> token =
			math_tokenize(math_list_begin, math_list_end, style);

		if(token.empty())
			return bounding_box_t(0, 0, 0, 0, 0, 0);

		std::vector<math_token_t>::const_iterator iterator =
			token.begin();
		bounding_box_t ret = iterator->_offset +
			iterator->_bounding_box;

		for(; iterator != token.end(); iterator++)
			ret = ret.merge(iterator->_offset +
							iterator->_bounding_box);

		return ret;
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const std::vector<math_text_t::item_t>::
			  const_iterator &math_list_begin,
			  const std::vector<math_text_t::item_t>::
			  const_iterator &math_list_end,
			  const unsigned int style,
			  const bool render_structure)
	{
		if(render_structure) {
			point(origin[0], origin[1]);
			rectangle(origin +
					  math_bounding_box(math_list_begin,
										math_list_end, style));
		}

		std::vector<math_text_t::item_t>::const_iterator
			math_list_begin_interior = math_list_begin;
		std::vector<math_text_t::item_t>::const_iterator
			math_list_end_interior = math_list_end;
		bool delimiter = false;

		if(math_list_begin->_type ==
		   math_text_t::item_t::TYPE_BOUNDARY &&
		   (math_list_end - 1)->_type ==
		   math_text_t::item_t::TYPE_BOUNDARY) {
			math_list_begin_interior++;
			math_list_end_interior--;
			delimiter = true;
		}

		std::vector<math_token_t> token =
			math_tokenize(math_list_begin, math_list_end, style);
		std::vector<math_token_t>::const_iterator token_iterator =
			token.begin();

		if(delimiter) {
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  math_list_begin->_atom._nucleus._math_symbol,
					  style, token_iterator->_delimiter_height,
					  render_structure);
			token_iterator++;
		}

		static const math_text_t::item_t fraction_item =
			math_text_t::item_t::TYPE_GENERALIZED_FRACTION;
		const std::vector<math_text_t::item_t>::const_iterator
			fraction_iterator =
			std::find(math_list_begin_interior,
					  math_list_end_interior, fraction_item);

		if(fraction_iterator != math_list_end_interior) {
			const float thickness = fraction_iterator->_length *
				default_rule_thickness * style_size(style);

			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  fraction_iterator + 1, math_list_end_interior,
					  token_iterator->_style,
					  render_structure);
			token_iterator++;
			if(thickness > 0) {
				filled_rectangle(
					origin +
					transform_pixel_to_logical().linear() *
					(token_iterator->_offset +
					 token_iterator->_bounding_box));
				token_iterator++;
			}
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  math_list_begin_interior, fraction_iterator,
					  token_iterator->_style,
					  render_structure);
			token_iterator++;
		}
		else
			// Incrementally process a math list
			for(std::vector<math_text_t::item_t>::const_iterator
					iterator = math_list_begin_interior;
				iterator != math_list_end_interior; iterator++) {
			switch(iterator->_type) {
			case math_text_t::item_t::TYPE_ATOM:
				if(render_structure)
					rectangle(origin +
							  transform_pixel_to_logical().linear() *
							  (token_iterator->_offset +
							   token_iterator->_bounding_box));
				math_text(origin +
						  transform_pixel_to_logical().linear() *
						  token_iterator->_offset,
						  iterator->_atom,
						  token_iterator->_style,
						  render_structure);
				token_iterator++;
				break;
			}
			// math_text_t::item_t::TYPE_KERN can be ignored
		}

		if(delimiter)
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  (math_list_end - 1)->
					  _atom._nucleus._math_symbol,
					  token_iterator->_style,
					  token_iterator->_delimiter_height,
					  render_structure);
	}

	bounding_box_t math_text_renderer_t::
	math_bounding_box(const math_text_t::field_t &field,
					  const unsigned int style)
	{
		switch(field._type) {
		case math_text_t::field_t::TYPE_MATH_SYMBOL:
			return math_bounding_box(field._math_symbol, style);
			break;
		case math_text_t::field_t::TYPE_BOX:
			return math_bounding_box(field._box, style);
			break;
		case math_text_t::field_t::TYPE_MATH_LIST:
			return math_bounding_box(field._math_list.begin(),
									 field._math_list.end(), style);
			break;
		default:
			return bounding_box_t(0, 0, 0, 0, 0, 0);
		}
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const math_text_t::field_t &field,
			  const unsigned int style, const bool render_structure)
	{
		switch(field._type) {
		case math_text_t::field_t::TYPE_MATH_SYMBOL:
			math_text(origin, field._math_symbol, style,
					  render_structure);
			break;
		case math_text_t::field_t::TYPE_BOX:
			math_text(origin, field._box, style, render_structure);
			break;
		case math_text_t::field_t::TYPE_MATH_LIST:
			math_text(origin, field._math_list.begin(),
					  field._math_list.end(), style,
					  render_structure);
			break;
		}
	}

	// TeX algorithm for (three-way) atoms:
	//
	// See Knuth, The TeXbook (1986), pp. 445f.
	bounding_box_t math_text_renderer_t::
	math_bounding_box(const math_text_t::atom_t &atom,
					  const unsigned int style)
	{
		std::vector<math_token_t> token =
			math_tokenize(atom, style);

		if(token.empty())
			return bounding_box_t(0, 0, 0, 0, 0, 0);

		std::vector<math_token_t>::const_iterator iterator =
			token.begin();
		bounding_box_t ret = iterator->_offset +
			iterator->_bounding_box;

		for(; iterator != token.end(); iterator++)
			ret = ret.merge(iterator->_offset +
							iterator->_bounding_box);

		return ret;
	}

	void math_text_renderer_t::
	math_text(const point_t origin,
			  const math_text_t::atom_t &atom,
			  const unsigned int style,
			  const bool render_structure)
	{
		const float x = origin[0];
		const float y = origin[1];

		if(render_structure) {
			point(x, y);
			rectangle(point_t(x, y) +
					  math_bounding_box(atom, style));
		}

		std::vector<math_token_t> token =
			math_tokenize(atom, style);
		std::vector<math_token_t>::const_iterator token_iterator =
			token.begin();

		bounding_box_t nucleus_bounding_box;

		// Rule 11
		if(atom._type == math_text_t::atom_t::TYPE_RAD) {
			if(!atom._index.empty()) {
				math_text(origin +
						  transform_pixel_to_logical().linear() *
						  token_iterator->_offset,
						  atom._index, token_iterator->_style,
						  render_structure);
				token_iterator++;
			}

			const math_text_t::math_symbol_t
				symbol_surd("\\surd", style);

			// Surd
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  symbol_surd, token_iterator->_style,
					  token_iterator->_delimiter_height,
					  render_structure);
			token_iterator++;
			// Rule with clearance
			filled_rectangle(
				origin + transform_pixel_to_logical().linear() *
				(token_iterator->_offset +
				 token_iterator->_bounding_box));
			// Skip the clearance token, too
			token_iterator += 2;
		}
		if(atom._type == math_text_t::atom_t::TYPE_OP &&
		   atom._nucleus._type ==
		   math_text_t::field_t::TYPE_MATH_SYMBOL)
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  atom._nucleus._math_symbol._glyph,
					  FAMILY_STIX_REGULAR,
					  style_size(style) * if_else_display(style,
							large_operator_display_scale, 1.0F),
					  render_structure);
		else
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  atom._nucleus, token_iterator->_style,
					  render_structure);

		if(atom._superscript.empty() && atom._subscript.empty())
			return;

		token_iterator++;

		if(atom._superscript.empty()) {
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  atom._subscript, token_iterator->_style,
					  render_structure);
			return;
		}
		if(atom._subscript.empty()) {
			math_text(origin +
					  transform_pixel_to_logical().linear() *
					  token_iterator->_offset,
					  atom._superscript, token_iterator->_style,
					  render_structure);
			return;
		}
		math_text(origin + transform_pixel_to_logical().linear() *
				  token_iterator->_offset,
				  atom._superscript, token_iterator->_style,
				  render_structure);
		token_iterator++;
		math_text(origin + transform_pixel_to_logical().linear() *
				  token_iterator->_offset,
				  atom._subscript, token_iterator->_style,
				  render_structure);
	}

	bounding_box_t math_text_renderer_t::
	bounding_box(const math_text_t &math_text)
	{
		if(!math_text.well_formed())
			bounding_box(L"*** invalid: " + math_text.code());

		return math_bounding_box(math_text._math_list._math_list,
								 default_style);
	}

	void math_text_renderer_t::
	text_raw(const float x, const float y,
			 const math_text_t &text)
	{
		if(!text.well_formed())
			text_raw(x, y, L"*** invalid: " + text.code());
		if(text._render_structure) {
			point(x, y);
			rectangle(point_t(x, y) + math_bounding_box(
				text._math_list._math_list, default_style));
		}
		math_text(point_t(x, y), text._math_list._math_list,
				  default_style, text._render_structure);
	}

}
