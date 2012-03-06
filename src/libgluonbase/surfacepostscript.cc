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

/////////////////////////////////////////////////////////////////////

namespace gluon {

	void postscript_surface_t::open_output_file(void)
	{
		_fp = fopen(_filename.c_str(), "w");
	}

	void postscript_surface_t::write_dsc_preamble(void) const
	{
		const std::string creator = "libgluon";

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
		if(_fp != NULL) {
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

		if(fp == NULL)
			return;
		if(fseek(fp, 0L, SEEK_END) == -1) {
			perror("fseek");
			return;
		}

		const long length = ftell(fp);

		if(length == -1) {
			perror("ftell");
			return;
		}
		font_data.resize(length);
		if(fseek(fp, 0L, SEEK_SET) == -1) {
			perror("fseek");
			return;
		}
		if(fread(&font_data[0], sizeof(FT_Byte), length, fp) !=
		   static_cast<size_t>(length)) {
			perror("fread");
			return;
		}
	}

	bool postscript_surface_t::
	parse_otf_cff_header(std::string &font_name,
						 unsigned int &cff_offset,
						 unsigned int &cff_length,
						 const std::vector<FT_Byte> font_data) const
	{
		// References:
		//
		// Adobe Systems, Inc., PostScript language Document
		// Structuring Convention specification (Adobe Systems, Inc.,
		// San Jose, CA, 1992), version 3.0, section 5.1.
		//
		// Adobe Systems, Inc., PostScript language reference manual
		// (Addison-Wesley, Reading, MA, 1999), 3rd edition, section
		// 5.8.1.
		//
		// Adobe Systems, Inc. and Microsoft Corp., OpenType
		// specification (2002), version 1.4.

		// OpenType file structure
		struct otf_offset_table_s {
			char sfnt_version[4];
			unsigned short num_tables;
			unsigned short search_range;
			unsigned short entry_selector;
			unsigned short range_shift;
		} offset_table;

		memcpy(&offset_table, &font_data[0],
			   sizeof(struct otf_offset_table_s));
		if(strncmp(offset_table.sfnt_version, "OTTO", 4) != 0)
			// Not a OpenType CFF/Type 2 font
			return false;
#ifdef LITTLE_ENDIAN
		offset_table.num_tables = bswap_16(offset_table.num_tables);
#endif // LITTLE_ENDIAN

		unsigned int name_offset = 0;
		//unsigned int name_length = 0;

		cff_offset = 0;
		cff_length = 0;

		for(unsigned short i = 0; i < offset_table.num_tables; i++) {
			struct otf_table_directory_s {
				char tag[4];
				unsigned int check_sum;
				unsigned int offset;
				unsigned int length;
			} table_directory;

			memcpy(&table_directory,
				   &font_data[sizeof(struct otf_offset_table_s) + i *
							  sizeof(struct otf_table_directory_s)],
				   sizeof(struct otf_table_directory_s));
#ifdef LITTLE_ENDIAN
			table_directory.offset = bswap_32(table_directory.offset);
			table_directory.length = bswap_32(table_directory.length);
#endif // LITTLE_ENDIAN
			if(strncmp(table_directory.tag, "name", 4) == 0) {
				name_offset = table_directory.offset;
				//name_length = table_directory.length;
			}
			else if(strncmp(table_directory.tag, "CFF ", 4) == 0) {
				cff_offset = table_directory.offset;
				cff_length = table_directory.length;
			}
		}

		if(name_offset == 0) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": invalid font file (no name table)"
					  << std::endl;
			return false;
		}
		else if(cff_offset == 0) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << "invalid font file (no CFF)" << std::endl;
			return false;
		}

		// name

		struct otf_naming_table_header_s {
			unsigned short format;
			unsigned short count;
			unsigned short string_offset;
		} naming_table_header;

		memcpy(&naming_table_header, &font_data[name_offset],
			   sizeof(struct otf_naming_table_header_s));
#ifdef LITTLE_ENDIAN
		naming_table_header.format =
			bswap_16(naming_table_header.format);
		naming_table_header.count =
			bswap_16(naming_table_header.count);
		naming_table_header.string_offset =
			bswap_16(naming_table_header.string_offset);
#endif // LITTLE_ENDIAN

		for(unsigned short i = 0; i < naming_table_header.count;
			i++) {
			struct otf_name_record_s {
				unsigned short platform_id;
				unsigned short encoding_id;
				unsigned short language_id;
				unsigned short name_id;
				unsigned short length;
				unsigned short offset;
			} name_record;
			const size_t base_offset = name_offset +
				sizeof(struct otf_naming_table_header_s);

			memcpy(&name_record,
				   &font_data[base_offset + i *
							  sizeof(struct otf_name_record_s)],
				   sizeof(struct otf_name_record_s));
#ifdef LITTLE_ENDIAN
			name_record.platform_id =
				bswap_16(name_record.platform_id);
			name_record.encoding_id =
				bswap_16(name_record.encoding_id);
			name_record.name_id = bswap_16(name_record.name_id);
#endif // LITTLE_ENDIAN
			// The font name in Mac OS Roman encoding is good enough
			// to obtain an ASCII PostScript name, while the Windows
			// platform uses a UTF-16 string that would require
			// conversion.
			if(name_record.platform_id == 1 &&
			   name_record.encoding_id == 0 &&
			   name_record.name_id == 6) {
#ifdef LITTLE_ENDIAN
				name_record.length = bswap_16(name_record.length);
				name_record.offset = bswap_16(name_record.offset);
#endif // LITTLE_ENDIAN

				char *buffer = new char[name_record.length + 1];

				memcpy(buffer,
					   &font_data[name_offset +
								  naming_table_header.string_offset +
								  name_record.offset],
					   name_record.length);
				buffer[name_record.length] = '\0';
				font_name = buffer;

				delete [] buffer;
			}
		}

		return true;
	}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)

	unsigned int postscript_surface_t::
	ascii85_line_count(const uint8_t *buffer, const size_t length)
		const
	{
		const unsigned int width = 64;
		unsigned int column = 0;
		unsigned int line = 0;

		for(size_t i = 0; i < length - 3; i += 4) {
			unsigned int b = reinterpret_cast<
				const unsigned int *>(buffer)[i >> 2];

			if(b == 0) {
				column++;
				if(column == width - 1) {
					line++;
					column = 0;
				}
			}
			else {
				if(column + 5 >= width) {
					column += 5 - width;
					line++;
				}
				else
					column += 5;
			}
		}
		if(column + (length & 3) + 3 >= width)
			line++;

		return line;
	}

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable: 810)
#endif // __INTEL_COMPILER
	void postscript_surface_t::
	append_ascii85(std::string &ascii, const uint8_t *buffer,
				   const size_t length) const
	{
		const int width = 64;
		int column = 0;

		for(size_t i = 0; i < length - 3; i += 4) {
			unsigned int dword = reinterpret_cast<
				const unsigned int *>(buffer)[i >> 2];

			if(dword == 0) {
				ascii.append(1, 'z');
				column++;
				if(column == width - 1) {
					ascii.append(1, '\n');
					column = 0;
				}
			}
			else {
#ifdef LITTLE_ENDIAN
				dword = bswap_32(dword);
#endif // LITTLE_ENDIAN

				char str[5];

				str[4] = static_cast<char>(dword % 85 + '!');
				dword /= 85;
				str[3] = static_cast<char>(dword % 85 + '!');
				dword /= 85;
				str[2] = static_cast<char>(dword % 85 + '!');
				dword /= 85;
				str[1] = static_cast<char>(dword % 85 + '!');
				dword /= 85;
				str[0] = static_cast<char>(dword % 85 + '!');
				for(int j = 0; j < 5; j++) {
					ascii.append(1, str[j]);
					column++;
					if(column == width) {
						ascii.append(1, '\n');
						column = 0;
					}
				}
			}
		}

		int k = length & 3;

		if(k > 0) {
			unsigned int dword = 0;

			memcpy(&dword, buffer + (length & ~3), k);
#ifdef LITTLE_ENDIAN
			dword = bswap_32(dword);
#endif // LITTLE_ENDIAN

			char str[5];

			str[4] = static_cast<char>(dword % 85 + '!');
			dword /= 85;
			str[3] = static_cast<char>(dword % 85 + '!');
			dword /= 85;
			str[2] = static_cast<char>(dword % 85 + '!');
			dword /= 85;
			str[1] = static_cast<char>(dword % 85 + '!');
			dword /= 85;
			str[0] = static_cast<char>(dword % 85 + '!');
			for(int j = 0; j < k + 1; j++) {
				ascii.append(1, str[j]);
				column++;
				if(column == width) {
					ascii.append(1, '\n');
					column = 0;
				}
			}

		}
		if(column > width - 2)
			ascii.append(1, '\n');
		ascii.append("~>");
	}
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif // __INTEL_COMPILER

#if defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
	std::string postscript_surface_t::
	font_embed_type_2(std::string &font_name,
					  const std::vector<FT_Byte> &font_data) const
	{
		// Embed an OpenType CFF (Type 2) file in ASCII85 encoding
		// with the PostScript syntax

		unsigned int cff_offset;
		unsigned int cff_length;

		if(!parse_otf_cff_header(font_name, cff_offset, cff_length,
								 font_data))
			return std::string();

		std::vector<FT_Byte> cff;

		cff.resize(cff_length + 10);
		memcpy(&cff[0], "StartData\r", 10);
		memcpy(&cff[10], &font_data[cff_offset], cff_length);

		char linebuf[BUFSIZ];
		std::string ret;

		snprintf(linebuf, BUFSIZ, "%%%%BeginResource: FontSet (%s)\n",
				 font_name.c_str());
		ret.append(linebuf);
		ret.append("%%VMusage: 0 0\n");
		ret.append("/FontSetInit /ProcSet findresource begin\n");
		snprintf(linebuf, BUFSIZ, "%%%%BeginData: %u ASCII Lines\n",
				 ascii85_line_count(&cff[0], cff_length) + 2);
		ret.append(linebuf);
		snprintf(linebuf, BUFSIZ,
				 "/%s %u currentfile /ASCII85Decode filter cvx exec\n",
				 font_name.c_str(), cff_length);
		ret.append(linebuf);
		append_ascii85(ret, &cff[0], cff_length + 10);
		ret.append(1, '\n');
		ret.append("%%EndData\n");
		ret.append("%%EndResource\n");

		return ret;
	}

	void postscript_surface_t::
	open_font_overwrite(const std::string &filename,
						const unsigned int family)
	{
		if(_font[family] != NULL)
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

		if(_fp != NULL)
			 embedded_str =
				 font_embed_type_2(_font_name[family],
								   _font_data[family]);

		if(_master) {
			bool embed_success = false;

			if(!embedded_str.empty()) {
				fputs(embedded_str.c_str(), _fp);
				embed_success = true;
			}
			if(!embed_success) {
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
		if(_font[family] == NULL)
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
		if(master) {
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
		for(unsigned int family = 0; family < NFAMILY; family++)
			if(_font[family] != NULL) {
				FT_Done_Face(_font[family]);
				_font[family] = NULL;
			}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
		if(_master) {
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
		if(_current_point_size != 0) {
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
		for(unsigned long i = 0; i < npixel; i++)
			for(unsigned long j = 0; j < 3; j++)
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
		if(string.empty() || _font[family] == NULL ||
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

		for(; iterator != string.end(); iterator++) {
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
		if(family >= NFAMILY || _font[family] == NULL) {
			std::cerr << __FILE__ << ':' << __LINE__
					  << ": error: font not initialized"
					  << std::endl;
			return;
		}
		if(_current_font_size[family] <= 0)
			return;
		// FIXME: Check if reloading/rescaling the font is necessary

		point_t transformed = _transform_logical_to_pixel * point_t(x, y);

		fprintf(_fp, "/%s findfont %.3f scalefont setfont\n"
				"%.3f %.3f moveto ", _font_name[family].c_str(),
				_current_font_size[family], transformed[0],
				screen_rect().bottom() - transformed[1]);

#include <table/adobeglyph.h>

		for(std::wstring::const_reverse_iterator iterator =
				string.rbegin();
			iterator != string.rend(); iterator++) {
			const wchar_t *lower =
				std::lower_bound(adobe_glyph_ucs,
								 adobe_glyph_ucs + nadobe_glyph,
								 *iterator);
			if(lower < adobe_glyph_ucs + nadobe_glyph &&
			   *lower == *iterator) {
				const unsigned long index =
					lower - adobe_glyph_ucs;

				fprintf(_fp, "/%s ", adobe_glyph_name[index]);
			}
			else
				fprintf(_fp, "/uni%04X ", *iterator);
		}
		// FIXME: extract 8-bit segment and use "show"
		if(string.size() == 1U)
			fprintf(_fp, "glyphshow\n");
		else
			fprintf(_fp, "%lu {glyphshow} repeat\n",
					string.size());
	}
#endif // defined(HAVE_FREETYPE2) || defined(HAVE_FTGL)
}
