/* Pronunciation Dictionary.
 *
 * Copyright (C) 2013-2015 Reece H. Dunn
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
#include "dictionary_format.hpp"

#include <ucd/ucd.h>
#include <cainteoir/unicode.hpp>
#include <cainteoir/mimetype.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace m = cainteoir::mime;

void tts::multiword_entry::operator++()
{
	++mCurrentWord;
	advance();
}

void tts::multiword_entry::advance()
{
	enum state_t
	{
		start,
		in_word,
	};

	state_t state = start;
	first = next;

	switch (mMode)
	{
	case hyphenated:
		for (; next < last; ++next) switch (*next)
		{
		case '-':
			switch (state)
			{
			case start:
				++first;
				state = in_word;
				mValue.stress = tts::initial_stress::as_transcribed;
				break;
			case in_word:
				mValue.word = std::make_shared<cainteoir::buffer>(first, next);
				return;
			}
			break;
		default:
			switch (state)
			{
			case start:
				state = in_word;
				mValue.stress = tts::initial_stress::as_transcribed;
				break;
			case in_word:
				break;
			}
			break;
		}
		break;
	case stressed:
		for (; next < last; ++next) switch (*next)
		{
		case '\'':
			switch (state)
			{
			case start:
				++first;
				state = in_word;
				mValue.stress = tts::initial_stress::primary;
				break;
			case in_word:
				mValue.word = std::make_shared<cainteoir::buffer>(first, next);
				return;
			}
			break;
		case ',':
			switch (state)
			{
			case start:
				++first;
				state = in_word;
				mValue.stress = tts::initial_stress::secondary;
				break;
			case in_word:
				mValue.word = std::make_shared<cainteoir::buffer>(first, next);
				return;
			}
			break;
		case '.':
			switch (state)
			{
			case start:
				++first;
				state = in_word;
				mValue.stress = tts::initial_stress::unstressed;
				break;
			case in_word:
				mValue.word = std::make_shared<cainteoir::buffer>(first, next);
				return;
			}
			break;
		default:
			switch (state)
			{
			case start:
				state = in_word;
				mValue.stress = tts::initial_stress::as_transcribed;
				break;
			case in_word:
				break;
			}
			break;
		}
		break;
	}

	if (first == last)
	{
		mValue.word = {};
		mValue.stress = tts::initial_stress::as_transcribed;
	}
	else
		mValue.word = std::make_shared<cainteoir::buffer>(first, next);
}

std::size_t tts::dictionary::key_hash::operator()(const key_type &a) const
{
	// DJB2 Hash Algorithm by Dan Bernstein:
	std::size_t hash = 5381;
	for (auto c : *a)
		hash = hash * 33 + c;
        return hash;
}

void tts::dictionary::add_entry(const key_type &aWord, const object &aEntry)
{
	object &entry = mEntries[aWord];
	if (entry.type() == object_type::null)
		entry = { object_type::array };
	entry.put(aEntry);
}

void tts::dictionary::add_entry(const key_type &aWord, const std::shared_ptr<buffer> &aEntry)
{
	object e{ object_type::dictionary };
	e.put("Entry::pronunciation", aEntry);
	add_entry(aWord, e);
}

void tts::dictionary::add_entry(const key_type &aWord, const ipa::phonemes &aEntry)
{
	object e{ object_type::dictionary };
	e.put("Entry::pronunciation", aEntry);
	add_entry(aWord, e);
}

const cainteoir::object &
tts::dictionary::lookup(const key_type &aWord) const
{
	static const object no_match = {};
	const auto &match = mEntries.find(aWord);
	return (match == mEntries.end()) ? no_match : match->second;
}

bool tts::dictionary::pronounce(const std::shared_ptr<buffer> &aWord,
                                const std::shared_ptr<tts::phoneme_reader> &aPronunciationRules,
                                ipa::phonemes &aPhonemes,
                                int depth)
{
	const auto &entry = lookup(aWord).get(0).get("Entry::pronunciation");
	switch (entry.type())
	{
	case object_type::phonemes: case object_type::phonemes_ref:
		aPhonemes = *entry.phonemes();
		return true;
	case object_type::buffer: case object_type::buffer_ref: // say-as entries
		if (depth == 5)
		{
			fprintf(stderr, "error: too much recursion for entry '%s'.\n", aWord->str().c_str());
			return false;
		}
		return pronounce(entry.buffer(), aPronunciationRules, aPhonemes, depth + 1);
	case object_type::null: // no match
		{
			multiword_entry words{ aWord, depth == 0
			                            ? multiword_entry::hyphenated : multiword_entry::stressed };
			if (words.is_multiword()) try
			{
				while (words.have_word())
				{
					const auto &entry = *words;

					if (words.position() != 0 &&
					    entry.stress == tts::initial_stress::unstressed)
						aPhonemes.push_back(ipa::syllable_break);

					ipa::phonemes phonemes;
					if (pronounce(entry.word, aPronunciationRules, phonemes, depth + 1))
						tts::make_stressed(phonemes, aPhonemes, entry.stress);
					else
						throw tts::phoneme_error("unable to pronounce the hyphenated word");

					++words;
				}
				return true;
			}
			catch (const tts::phoneme_error &e)
			{
				aPhonemes.clear();
			}
		}
		if (aPronunciationRules.get()) try
		{
			aPronunciationRules->reset(aWord);
			while (aPronunciationRules->read())
				aPhonemes.push_back(*aPronunciationRules);
			return true;
		}
		catch (const tts::phoneme_error &e)
		{
			// Unable to pronounce the word using the ruleset, so fall
			// through to the failure logic below.
		}
		break;
	}

	return false;
}

struct dictionary_entry_formatter : public tts::dictionary_formatter
{
	dictionary_entry_formatter(FILE *aOut)
		: mOut(aOut)
	{
	}

	void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                         std::shared_ptr<tts::phoneme_writer> &writer,
	                         const ipa::phonemes &phonemes,
	                         const cainteoir::object &entry,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const cainteoir::object &entry,
	                        const char *line_separator);

	FILE *mOut;
};

void dictionary_entry_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                     std::shared_ptr<tts::phoneme_writer> &writer,
                                                     const ipa::phonemes &phonemes,
                                                     const cainteoir::object &entry,
                                                     const char *line_separator)
{
	fprintf(stdout, "\"%s\" => /", word->str().c_str());
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fprintf(stdout, "/ [%s]%s", writer->name(), line_separator);
}

void dictionary_entry_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                    const std::shared_ptr<cainteoir::buffer> &say_as,
                                                    const cainteoir::object &entry,
                                                    const char *line_separator)
{
	ucd::codepoint_t cp = 0;
	cainteoir::utf8::read(say_as->begin(), cp);

	fprintf(stdout, "\"%s\" => \"%s\"@%s [say-as]%s",
	        word->str().c_str(),
	        say_as->str().c_str(),
	        ucd::get_script_string(ucd::lookup_script(cp)),
	        line_separator);
}

std::shared_ptr<tts::dictionary_formatter> tts::createDictionaryEntryFormatter(FILE *out)
{
	return std::make_shared<dictionary_entry_formatter>(out);
}

std::shared_ptr<tts::dictionary_reader> tts::createDictionaryReader(const char *aDictionaryPath)
{
	if (!aDictionaryPath) return {};

	char data[128] = { 0 };
	FILE *f = fopen(aDictionaryPath, "rb");
	if (!f) return {};

	size_t n = f ? fread(data, 1, sizeof(data), f) : 0;
	fclose(f);

	auto header = std::make_shared<cainteoir::buffer>(data, data + n);
	if (m::cainteoir.match(header))
		return createCainteoirDictionaryReader(aDictionaryPath);
	if (m::cmudict.match(header))
		return createCMUDictionaryReader(aDictionaryPath);

	return {};
}

std::shared_ptr<tts::dictionary_formatter> tts::createDictionaryFormatter(FILE *out, const char *aFormat)
{
	if (!aFormat)
		return tts::createDictionaryEntryFormatter(out);
	if (!strcmp(aFormat, "cainteoir"))
		return tts::createCainteoirDictionaryFormatter(out);
	if (!strcmp(aFormat, "cmudict"))
		return tts::createCMUDictionaryFormatter(out);
	if (!strcmp(aFormat, "espeak"))
		return tts::createEspeakDictionaryFormatter(out);
	return {};
}

void tts::formatDictionary(tts::dictionary &dict,
	                   std::shared_ptr<dictionary_formatter> &formatter,
	                   std::shared_ptr<phoneme_writer> &writer,
	                   bool resolve_say_as_entries)
{
	for (auto &entry : dict)
	{
		size_t n = entry.second.size();
		for (size_t i = 0; i != n; ++i)
		{
			const auto &e = entry.second.get(i);
			const auto &pronunciation = e.get("Entry::pronunciation");
			if (pronunciation.is_phonemes())
				formatter->write_phoneme_entry(entry.first, writer, *pronunciation.phonemes(), e);
			else if (resolve_say_as_entries)
			{
				ipa::phonemes pronunciation;
				if (dict.pronounce(entry.first, {}, pronunciation))
					formatter->write_phoneme_entry(entry.first, writer, pronunciation, e);
			}
			else
				formatter->write_say_as_entry(entry.first, pronunciation.buffer(), e);
		}
	}
}

void tts::formatDictionary(tts::dictionary &dict,
	                   std::shared_ptr<dictionary_formatter> &formatter,
	                   std::shared_ptr<phoneme_reader> &reader,
	                   std::shared_ptr<phoneme_writer> &writer)
{
	static const object null_entry;

	for (auto &entry : dict)
	{
		ipa::phonemes pronunciation;
		reader->reset(entry.first);
		while (reader->read())
			pronunciation.push_back(*reader);

		formatter->write_phoneme_entry(entry.first, writer, pronunciation, null_entry);
	}
}
