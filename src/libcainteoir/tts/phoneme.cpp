/* Phoneme Model.
 *
 * Copyright (C) 2013 Reece H. Dunn
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
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/phoneme.hpp>
#include <utility>
#include <string.h>

namespace tts = cainteoir::tts;

static std::initializer_list<const std::pair<const char *, tts::feature>> abbreviations = {
	{ "alv", tts::feature::alveolar },
	{ "apr", tts::feature::approximant },
	{ "asp", tts::feature::aspirated },
	{ "bck", tts::feature::back },
	{ "blb", tts::feature::bilabial },
	{ "clk", tts::feature::click },
	{ "cnt", tts::feature::center },
	{ "dnt", tts::feature::dental },
	{ "ejc", tts::feature::ejective },
	{ "flp", tts::feature::flap },
	{ "fnt", tts::feature::front },
	{ "frc", tts::feature::fricative },
	{ "fzd", tts::feature::pharyngealized },
	{ "glt", tts::feature::glottal },
	{ "hgh", tts::feature::high },
	{ "imp", tts::feature::implosive },
	{ "lat", tts::feature::lateral },
	{ "lbd", tts::feature::labio_dental },
	{ "lbv", tts::feature::labio_velar },
	{ "lmd", tts::feature::lower_mid },
	{ "lng", tts::feature::long_ },
	{ "low", tts::feature::low },
	{ "lzd", tts::feature::labialized },
	{ "mid", tts::feature::mid },
	{ "mrm", tts::feature::murmured },
	{ "nas", tts::feature::nasal },
	{ "nzd", tts::feature::nasalized },
	{ "pal", tts::feature::palatal },
	{ "phr", tts::feature::pharyngeal },
	{ "pla", tts::feature::palato_alveolar },
	{ "pzd", tts::feature::palatalized },
	{ "rfx", tts::feature::retroflex },
	{ "rnd", tts::feature::rounded },
	{ "rzd", tts::feature::rhoticized },
	{ "smh", tts::feature::semi_high },
	{ "stp", tts::feature::plosive },
	{ "syl", tts::feature::syllabic },
	{ "trl", tts::feature::trill },
	{ "umd", tts::feature::upper_mid },
	{ "unr", tts::feature::unrounded },
	{ "unx", tts::feature::unexploded },
	{ "uvl", tts::feature::uvular },
	{ "vcd", tts::feature::voiced },
	{ "vel", tts::feature::velar },
	{ "vls", tts::feature::voiceless },
	{ "vwl", tts::feature::vowel },
	{ "vzd", tts::feature::velarized },
};

tts::phoneme::phoneme(const feature a, const feature b, const feature c, const feature d, const feature e)
{
	features[0] = a;
	features[1] = b;
	features[2] = c;
	features[3] = d;
	features[4] = e;
}

bool tts::phoneme::contains(const feature f) const
{
	if (f != feature::unspecified) for (const feature x : features)
	{
		if (x == f) return true;
	}
	return false;
}

bool tts::phoneme::operator==(const phoneme &rhs) const
{
	return *(const uint32_t *)features == *(const uint32_t *)rhs.features &&
	       features[4] == rhs.features[4];
}

bool tts::phoneme::operator!=(const phoneme &rhs) const
{
	return *(const uint32_t *)features != *(const uint32_t *)rhs.features ||
	       features[4] != rhs.features[4];
}

tts::feature tts::get_feature_id(const char *abbreviation)
{
	if (!abbreviation) return tts::feature::unspecified;

	int begin = 0;
	int end = abbreviations.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;
		auto &item = *(abbreviations.begin() + pos);

		int comp = strcmp(item.first, abbreviation);
		if (comp == 0)
			return (abbreviations.begin() + pos)->second;
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return tts::feature::unspecified;
}

tts::phoneme_reader::phoneme_reader()
	: phoneme(tts::feature::unspecified, tts::feature::unspecified, tts::feature::unspecified)
{
}

struct explicit_feature_reader : public tts::phoneme_reader
{
	void reset(const cainteoir::buffer &aBuffer);

	bool read();

	enum state_t
	{
		begin_phoneme,
		in_phoneme,
		in_feature,
	};

	const char *mCurrent;
	const char *mEnd;
};

void explicit_feature_reader::reset(const cainteoir::buffer &aBuffer)
{
	mCurrent = aBuffer.begin();
	mEnd = aBuffer.end();
}

bool explicit_feature_reader::read()
{
	state_t s = begin_phoneme;
	char abbrev[4] = { '\0', '\0', '\0', '\0' };
	int abbrev_pos = 0;
	tts::feature features[5] = {
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
	};
	int feature_pos = 0;

	while (mCurrent < mEnd)
	{
		switch (s)
		{
		case begin_phoneme:
			switch (*mCurrent)
			{
			case '{':
				s = in_phoneme;
				break;
			case '\0':
				break;
			default:
				throw tts::phoneme_error(i18n("unexpected start of phoneme (expecting '{')"));
			}
			break;
		case in_phoneme:
			switch (*mCurrent)
			{
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
			case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
			case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
				s = in_feature;
				abbrev_pos = 0;
				abbrev[abbrev_pos] = *mCurrent;
				break;
			case ',': case '}':
				throw tts::phoneme_error(i18n("missing phoneme feature"));
			}
			break;
		case in_feature:
			switch (*mCurrent)
			{
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
			case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
			case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
				++abbrev_pos;
				if (abbrev_pos <= 2)
					abbrev[abbrev_pos] = *mCurrent;
				break;
			case ',': case '}':
				if (abbrev_pos != 2)
					throw tts::phoneme_error(i18n("a phoneme feature must be 3 characters long"));

				if (feature_pos < 5)
				{
					features[feature_pos] = tts::get_feature_id(abbrev);
					if (features[feature_pos] != tts::feature::unspecified)
						++feature_pos;
				}
				else
					throw tts::phoneme_error(i18n("a phoneme must specify no more than 5 features"));

				if (*mCurrent == ',')
					s = in_phoneme;
				else
				{
					if (feature_pos <= 2)
						throw tts::phoneme_error(i18n("a phoneme must specify at least 3 features"));

					*(tts::phoneme *)this = { features[0], features[1], features[2], features[3], features[4] };
					++mCurrent;
					return true;
				}
				break;
			}
			break;
		}
		++mCurrent;
	}

	if (s != begin_phoneme)
		throw tts::phoneme_error(i18n("unexpected end of phoneme (expecting '}')"));

	*(tts::phoneme *)this = { features[0], features[1], features[2], features[3], features[4] };
	return false;
}

std::shared_ptr<tts::phoneme_reader> tts::createPhonemeReader()
{
	return std::make_shared<explicit_feature_reader>();
}
