/* Document Content Rendering Model.
 *
 * Copyright (C) 2012 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/content.hpp>
#include <stdexcept>
#include <sstream>
#include <limits>

namespace css = cainteoir::css;

struct css_reader
{
	enum token_type
	{
		identifier,
		at_keyword,
		string,
		integer,
		open_block,
		close_block,
		colon,
		semicolon,
		comma,
		error,
	};

	cainteoir::buffer value;
	token_type type;

	bool read();

	css_reader(std::shared_ptr<cainteoir::buffer> aData);
private:
	void parse_identifier(token_type aType);

	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
};

#define _  0 // error
#define S  1 // whitespace
#define B  2 // open  curly brace
#define E  3 // close curly brace
#define A  4 // at
#define L  5 // letter
#define N  6 // number
#define C  7 // colon
#define c  8 // semicolon
#define Q  9 // double-quote
#define q 10 // single-quote
#define M 11 // hyphen/minus
#define F 12 // forward slash
#define s 13 // comma

static const char css_lookup_table[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, S, S, S, S, S, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ S, _, Q, _, _, _, _, q, _, _, _, _, s, M, _, F,
	/* 3x */ N, N, N, N, N, N, N, N, N, N, C, c, _, _, _, _,
	/* 4x */ A, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,
	/* 5x */ L, L, L, L, L, L, L, L, L, L, L, _, _, _, _, _,
	/* 6x */ _, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,
	/* 7x */ L, L, L, L, L, L, L, L, L, L, L, B, _, E, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

css_reader::css_reader(std::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mCurrent(mData->begin())
	, value(nullptr, nullptr)
{
}

bool css_reader::read()
{
	type = error;
	value = cainteoir::buffer(nullptr, nullptr);

	if (mCurrent >= mData->end())
		return false;

	const char *start = mCurrent;
	while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent++])
	{
	case S: start = mCurrent; break;
	case B: type  = open_block;  return true;
	case E: type  = close_block; return true;
	case C: type  = colon;       return true;
	case c: type  = semicolon;   return true;
	case s: type  = comma;       return true;
	case A:
		type = at_keyword;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case L:
		case N:
		case M:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case N:
	case M:
		type = integer;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case N:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case L:
		type = identifier;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case L:
		case N:
		case M:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case q:
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		default:
			++mCurrent;
			break;
		case q:
			value = cainteoir::buffer(start+1, mCurrent);
			type  = string;
			++mCurrent;
			return true;
		}
		value = cainteoir::buffer(start+1, mCurrent);
		type  = error;
		return true;
	case F:
		if (*mCurrent == '*')
		{
			++mCurrent;
			++mCurrent;
			while (mCurrent[0] != '*' && mCurrent[1] != '/')
				++mCurrent;
			++mCurrent;
			++mCurrent;
			start = mCurrent;
		}
		else
		{
			value = cainteoir::buffer(start, mCurrent);
			type  = error;
			return true;
		}
		break;
	default:
		value = cainteoir::buffer(start, mCurrent);
		type  = error;
		return true;
	}

	return false;
}

#undef _
#undef S
#undef B
#undef E
#undef A
#undef L
#undef N
#undef C
#undef c
#undef Q
#undef q
#undef M
#undef F
#undef s

css::length css::length::as(const type aUnits) const
{
	static constexpr float points_in_pica = 12;
	static constexpr float points_in_inch = 72;
	static constexpr float picas_in_inch = points_in_inch / points_in_pica;
	static constexpr float centimeters_in_inch = 2.54;
	static constexpr float centimeters_in_point = centimeters_in_inch / points_in_inch;
	static constexpr float centimeters_in_pica = centimeters_in_inch / picas_in_inch;
	static constexpr float millimeters_in_centimeter = 10;
	static constexpr float millimeters_in_inch = centimeters_in_inch * millimeters_in_centimeter;
	static constexpr float millimeters_in_point = millimeters_in_inch / points_in_inch;
	static constexpr float millimeters_in_pica  = millimeters_in_inch / picas_in_inch;
	static constexpr float pixels_in_inch = 96;
	static constexpr float pixels_in_point = pixels_in_inch / points_in_inch;
	static constexpr float pixels_in_pica = pixels_in_inch / picas_in_inch;
	static constexpr float pixels_in_centimeter = pixels_in_inch / centimeters_in_inch;
	static constexpr float pixels_in_millimeter = pixels_in_inch / millimeters_in_inch;

	switch (mUnits)
	{
	case millimeters:
		switch (aUnits)
		{
		case millimeters:
			return *this;
		case centimeters:
			return length(mValue / millimeters_in_centimeter, aUnits);
		case inches:
			return length(mValue / millimeters_in_inch, aUnits);
		case points:
			return length(mValue / millimeters_in_point, aUnits);
		case picas:
			return length(mValue / millimeters_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_millimeter, aUnits);
		}
		break;
	case centimeters:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_centimeter, aUnits);
		case centimeters:
			return *this;
		case inches:
			return length(mValue / centimeters_in_inch, aUnits);
		case points:
			return length(mValue / centimeters_in_point, aUnits);
		case picas:
			return length(mValue / centimeters_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_centimeter, aUnits);
		}
		break;
	case inches:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_inch, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_inch, aUnits);
		case inches:
			return *this;
		case points:
			return length(mValue * points_in_inch, aUnits);
		case picas:
			return length(mValue * picas_in_inch, aUnits);
		case pixels:
			return length(mValue * pixels_in_inch, aUnits);
		}
		break;
	case points:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_point, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_point, aUnits);
		case inches:
			return length(mValue / points_in_inch, aUnits);
		case points:
			return *this;
		case picas:
			return length(mValue / points_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_point, aUnits);
		}
		break;
	case picas:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_pica, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_pica, aUnits);
		case inches:
			return length(mValue / picas_in_inch, aUnits);
		case points:
			return length(mValue * points_in_pica, aUnits);
		case picas:
			return *this;
		case pixels:
			return length(mValue * pixels_in_pica, aUnits);
		}
		break;
	case pixels:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue / pixels_in_millimeter, aUnits);
		case centimeters:
			return length(mValue / pixels_in_centimeter, aUnits);
		case inches:
			return length(mValue / pixels_in_inch, aUnits);
		case points:
			return length(mValue / pixels_in_point, aUnits);
		case picas:
			return length(mValue / pixels_in_pica, aUnits);
		case pixels:
			return *this;
		}
		break;
	}

	throw std::runtime_error("unable to convert to the specified units");
}

std::string css::counter_style::marker(value_t count) const
{
	std::ostringstream textval;
	textval << prefix;
	int n = symbols.size();
	if (n != 0) switch (system)
	{
	case css::counter_system::cyclic:
		textval << symbols[count % n];
		break;
	case css::counter_system::numeric:
		if (count == 0)
			textval << symbols[0];
		else
		{
			std::string s;
			while (count != 0)
			{
				s = symbols[count % n] + s;
				count = count / n;
			}
			textval << s;
		}
		break;
	}
	textval << suffix << ' ';
	return textval.str();
}

const css::counter_style::range_t css::counter_style::infinite = {
	std::numeric_limits<value_t>::min(),
	std::numeric_limits<value_t>::max()
};

const css::counter_style::range_t css::counter_style::get_auto_range(counter_system system)
{
	switch (system)
	{
	case counter_system::alphabetic:
	case counter_system::symbolic:
		return { 1, infinite.second };
	case counter_system::additive:
		return { 0, infinite.second };
	}
	return infinite;
}

const css::counter_style *css::style_manager::get_counter_style(const std::string &aName) const
{
	auto item = mCounterStyles.find(aName);
	if (item != mCounterStyles.end())
		return item->second;
	return nullptr;
}

css::counter_style *css::style_manager::create_counter_style(const std::string &aName)
{
	std::shared_ptr<counter_style> style = std::make_shared<counter_style>();
	mCounterStyleRegistry.push_back(style);
	mCounterStyles[aName] = style.get();
	return style.get();
}

static void parse_counter_style(css_reader &css, css::counter_style *style)
{
	while (css.read())
	{
		if (css.type == css_reader::close_block)
			return;

		if (css.type != css_reader::identifier)
			continue;

		cainteoir::buffer name = css.value;
		if (!css.read() || css.type != css_reader::colon || !css.read())
			continue;

		if (name.comparei("system") == 0 && css.type == css_reader::identifier)
		{
			if (css.value.comparei("cyclic") == 0)
				style->system = css::counter_system::cyclic;
			else if (css.value.comparei("fixed") == 0)
			{
				style->system = css::counter_system::fixed;
				if (css.read() && css.type == css_reader::integer)
					style->initial_symbol_value = atoi(css.value.begin());
			}
			else if (css.value.comparei("symbolic") == 0)
				style->system = css::counter_system::symbolic;
			else if (css.value.comparei("alphabetic") == 0)
				style->system = css::counter_system::alphabetic;
			else if (css.value.comparei("numeric") == 0)
				style->system = css::counter_system::numeric;
			else if (css.value.comparei("additive") == 0)
				style->system = css::counter_system::additive;
			style->range = css::counter_style::get_auto_range(style->system);
		}
		else if (name.comparei("negative") == 0 && css.type == css_reader::string)
		{
			style->negative_prefix = css.value.str();
			if (css.read() && css.type == css_reader::string)
				style->negative_suffix = css.value.str();
		}
		else if (name.comparei("prefix") == 0 && css.type == css_reader::string)
			style->prefix = css.value.str();
		else if (name.comparei("suffix") == 0 && css.type == css_reader::string)
			style->suffix = css.value.str();
		else if (name.comparei("range") == 0)
		{
			if (css.type == css_reader::identifier && css.value.comparei("auto") == 0)
				style->range = css::counter_style::get_auto_range(style->system);
			else
			{
				bool valid_range = true;
				css::counter_style::value_t values[2] = { 0, 0 };
				for (int i = 0; i < 2 && valid_range; ++i)
				{
					if (css.type == css_reader::integer)
						values[i] = atoi(css.value.begin());
					else if (css.type == css_reader::identifier && css.value.comparei("infinite") == 0)
						values[i] = (i == 0) ? css::counter_style::infinite.first : css::counter_style::infinite.second;
					else
						valid_range = false;

					if (valid_range)
						css.read();
				}

				if (valid_range && values[0] <= values[1])
					style->range = { values[0], values[1] };
			}
		}
		else if (name.comparei("symbols") == 0 && css.type == css_reader::string)
		{
			style->symbols.clear();
			while (css.type == css_reader::string)
			{
				style->symbols.push_back(css.value.str());
				css.read();
			}
		}
		else if (name.comparei("additive-symbols") == 0 && css.type == css_reader::integer)
		{
			style->additive_symbols.clear();
			while (css.type == css_reader::integer)
			{
				std::pair<css::counter_style::value_t, std::string> value;
				value.first = atoi(css.value.begin());
				if (css.read() && css.type == css_reader::string)
				{
					value.second = css.value.str();
					style->additive_symbols.push_back(value);
					if (css.read() && css.type == css_reader::comma)
						css.read();
				}
			}
		}

		while (css.type != css_reader::semicolon)
			css.read();
	}
}

void css::style_manager::parse(const char *css_file)
{
	const char *datadir = getenv("CAINTEOIR_DATA_DIR");
	if (!datadir)
		datadir = DATADIR "/" PACKAGE;

	const std::string filename = datadir + std::string(css_file);
	parse(make_file_buffer(filename.c_str()));
}

void css::style_manager::parse(const std::shared_ptr<buffer> &style)
{
	css_reader css(style);
	while (css.read())
	{
		if (css.type == css_reader::at_keyword && css.value.comparei("@counter-style") == 0 &&
		    css.read() && css.type == css_reader::identifier)
		{
			css::counter_style *style = create_counter_style(css.value.str());
			if (css.read() && css.type == css_reader::open_block)
				parse_counter_style(css, style);
		}
	}
}

const css::styles cainteoir::unknown =
{
	"unknown",
	css::display::inherit,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::paragraph =
{
	"paragraph",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::paragraph,
	0,
};

const css::styles cainteoir::heading0 =
{
	"heading0",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	0,
};

const css::styles cainteoir::heading1 =
{
	"heading1",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	1,
};

const css::styles cainteoir::heading2 =
{
	"heading2",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1.5, css::length::picas },
	{ {}, {}, {}, { 2, css::length::picas } },
	css::text_structure::heading,
	2,
};

const css::styles cainteoir::heading3 =
{
	"heading3",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	3,
};

const css::styles cainteoir::heading4 =
{
	"heading4",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	4,
};

const css::styles cainteoir::heading5 =
{
	"heading5",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	5,
};

const css::styles cainteoir::heading6 =
{
	"heading6",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	6,
};

const css::styles cainteoir::span =
{
	"span",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::sentence =
{
	"sentence",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::sentence,
	0,
};

const css::styles cainteoir::superscript =
{
	"superscript",
	css::display::inlined,
	css::vertical_align::super,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::subscript =
{
	"subscript",
	css::display::inlined,
	css::vertical_align::sub,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::emphasized =
{
	"emphasized",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::italic,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::emphasized_block =
{
	"emphasized_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::italic,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::paragraph,
	0,
};

const css::styles cainteoir::strong =
{
	"strong",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::reduced =
{
	"reduced",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::underlined =
{
	"underlined",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::underline,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::monospace =
{
	"monospace",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::monospace_block =
{
	"monospace_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::bullet_list =
{
	"bullet_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"disc",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::number_list =
{
	"number_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"decimal",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::list_item =
{
	"list_item",
	css::display::list_item,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table =
{
	"table",
	css::display::table,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table_row =
{
	"table_row",
	css::display::table_row,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table_cell =
{
	"table_cell",
	css::display::table_cell,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};