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

#ifndef GLUON_STATE_H_
#define GLUON_STATE_H_

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <utility>
#include <gluon/fixpoint.h>

namespace gluon {

	class polygon_t;

	// Adobe (1999), section 4.2, 7.5.2, 8.2
	class graphics_state_t {
	public:
		typedef std::pair<std::vector<float>, float> dash_pattern_t;
		enum {
			LINE_CAP_BUTT = 0,
			LINE_CAP_ROUND,
			LINE_CAP_PROJECTING_SQUARE
		};
		enum {
			LINE_JOIN_MITER = 0,
			LINE_JOIN_ROUND,
			LINE_JOIN_BEVEL
		};
	private:
		float _line_width;
		unsigned int _line_cap;
		unsigned int _line_join;
		float _miter_limit;
		dash_pattern_t _dash_pattern;
		bool _stroke_adjustment;
	public:
		graphics_state_t(void)
			: _line_width(20.0), _line_cap(LINE_CAP_BUTT),
			  _line_join(LINE_JOIN_MITER), _miter_limit(10.0),
			  _stroke_adjustment(false)
		{
			static const float zero = 0;

			_dash_pattern =
				dash_pattern_t(std::vector<float>(), zero);
			if(_miter_limit < _line_width)
				_miter_limit = 10 * _line_width;

			//I(_miter_limit >= 1.0);
		}
		friend class polygon_t;
	};
}

#endif // GLUON_STATE_H_
