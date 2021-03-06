/* Phoneme Stress Patterns.
 *
 * Copyright (C) 2014 Reece H. Dunn
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
namespace ipa = cainteoir::ipa;

enum syllable_t : uint8_t
{
	start,
	onset,
	nucleus,
	coda_start,
	coda_start_affricate,
	coda,
	coda_affricate,
};

struct syllable_reader_t : public tts::syllable_reader
{
	void reset(ipa::phonemes &aPhonemes);

	bool read();

	ipa::phonemes::iterator last;
};

void syllable_reader_t::reset(ipa::phonemes &aPhonemes)
{
	end  = aPhonemes.begin();
	last = aPhonemes.end();
}

bool syllable_reader_t::read()
{
	syllable_t state = syllable_t::start;
	ipa::phonemes::iterator coda_start = end;
	onset = end;

	bool have_syllable = false;
	for (auto current = onset; current != last; ++current)
	{
		auto &phoneme = *current;
		switch (phoneme.get(ipa::phoneme_type | ipa::syllabicity))
		{
		case ipa::consonant | ipa::syllabic:
		case ipa::vowel:
			have_syllable = true;
			switch (state)
			{
			case syllable_t::start:
			case syllable_t::onset:
				state = syllable_t::nucleus;
				nucleus = current;
				break;
			case syllable_t::nucleus:
				coda = end = current;
				return true;
			case syllable_t::coda_start_affricate:
			case syllable_t::coda_start:
				coda = end = coda_start;
				return true;
			case syllable_t::coda:
				end = coda_start;
				coda = --coda_start;
				return true;
			case syllable_t::coda_affricate:
				coda = --coda_start;
				++coda_start;
				end = ++coda_start;
				return true;
			}
			break;
		case ipa::vowel | ipa::non_syllabic:
			have_syllable = true;
			switch (state)
			{
			case syllable_t::start:
			case syllable_t::onset:
				state = syllable_t::nucleus;
				nucleus = current;
				break;
			case syllable_t::nucleus:
				break;
			case syllable_t::coda_start_affricate:
			case syllable_t::coda_start:
				coda = end = coda_start;
				return true;
			case syllable_t::coda:
				end = coda_start;
				coda = --coda_start;
				return true;
			case syllable_t::coda_affricate:
				coda = --coda_start;
				++coda_start;
				end = ++coda_start;
				return true;
			}
			break;
		case ipa::separator:
			switch (state)
			{
			case syllable_t::start:
			case syllable_t::onset:
				onset = coda_start = current;
				break;
			case syllable_t::nucleus:
				coda = current;
				end = current;
				return true;
			case syllable_t::coda_start_affricate:
			case syllable_t::coda_start:
				coda = coda_start;
				end = current;
				return true;
			case syllable_t::coda:
			case syllable_t::coda_affricate:
				coda = --coda_start;
				end = current;
				return true;
			}
			break;
		case ipa::consonant:
			have_syllable = true;
			switch (state)
			{
			case syllable_t::start:
				onset = current;
				state = syllable_t::onset;
				break;
			case syllable_t::onset:
				break;
			case syllable_t::nucleus:
				if (phoneme.get(ipa::joined_to_next_phoneme) == ipa::joined_to_next_phoneme)
					state = syllable_t::coda_start_affricate;
				else
					state = syllable_t::coda_start;
				coda_start = current;
				break;
			case syllable_t::coda_start_affricate:
				state = syllable_t::coda_affricate;
				coda_start = current;
				break;
			case syllable_t::coda_start:
				state = syllable_t::coda;
				coda_start = current;
				break;
			case syllable_t::coda_affricate:
			case syllable_t::coda:
				break;
			}
			break;
		default:
			switch (state)
			{
			case syllable_t::start:
			case syllable_t::onset:
				onset = coda_start = current;
				break;
			case syllable_t::nucleus:
				coda = current;
				end = current;
				return true;
			case syllable_t::coda_start_affricate:
			case syllable_t::coda_start:
				coda = coda_start;
				end = current;
				return true;
			case syllable_t::coda:
			case syllable_t::coda_affricate:
				coda = --coda_start;
				end = current;
				return true;
			}
			break;
		}
	}

	if (have_syllable) switch (state)
	{
	case syllable_t::nucleus:
		coda = end = last;
		return true;
	case syllable_t::coda_start:
		coda = coda_start;
		end = last;
		return true;
	case syllable_t::coda:
	case syllable_t::coda_affricate:
		end = last;
		coda = --coda_start;
		return true;
	}

	onset = nucleus = coda = end = last;
	return false;
}

std::shared_ptr<tts::syllable_reader> tts::create_syllable_reader()
{
	return std::make_shared<syllable_reader_t>();
}

static void make_vowel_stressed(ipa::phonemes &aPhonemes)
{
	syllable_reader_t syllable;
	syllable.reset(aPhonemes);
	while (syllable.read())
	{
		auto stress = syllable.onset->get(ipa::stress);
		if (stress != ipa::unstressed)
		{
			syllable.onset->set(ipa::unstressed, ipa::stress);
			syllable.nucleus->set(stress, ipa::stress);
		}
	}
}

static void make_syllable_stressed(ipa::phonemes &aPhonemes)
{
	syllable_reader_t syllable;
	syllable.reset(aPhonemes);
	while (syllable.read())
	{
		auto stress = syllable.nucleus->get(ipa::stress);
		if (stress != ipa::unstressed)
		{
			syllable.nucleus->set(ipa::unstressed, ipa::stress);
			syllable.onset->set(stress, ipa::stress);
		}
	}
}

static void make_primary_stressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(ipa::primary_stress, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

static void make_secondary_stressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(ipa::secondary_stress, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

static void make_initial_unstressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(0, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

void tts::make_stressed(ipa::phonemes &aPhonemes, stress_type aType)
{
	switch (aType)
	{
	case stress_type::as_transcribed:
		break; // no change
	case stress_type::vowel:
		make_vowel_stressed(aPhonemes);
		break;
	case stress_type::syllable:
		make_syllable_stressed(aPhonemes);
		break;
	}
}

void tts::make_stressed(const ipa::phonemes &aPhonemes,
                        ipa::phonemes &aOutput,
                        initial_stress aType)
{
	switch (aType)
	{
	case initial_stress::as_transcribed:
		for (const auto &p : aPhonemes)
			aOutput.push_back(p);
		break;
	case initial_stress::primary:
		make_primary_stressed(aPhonemes, aOutput);
		break;
	case initial_stress::secondary:
		make_secondary_stressed(aPhonemes, aOutput);
		break;
	case initial_stress::unstressed:
		make_initial_unstressed(aPhonemes, aOutput);
		break;
	}
}
