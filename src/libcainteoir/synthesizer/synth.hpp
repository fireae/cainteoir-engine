/* Text-to-Speech Synthesizer Internal API.
 *
 * Copyright (C) 2014-2015 Reece H. Dunn
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

#ifndef CAINTEOIR_SYNTHESIZER_SYNTH_HPP
#define CAINTEOIR_SYNTHESIZER_SYNTH_HPP

#include <cainteoir/synthesizer.hpp>

namespace cainteoir { namespace tts
{
	// VoiceDB Data Parser Helpers /////////////////////////////////////////////////////////

	static inline constexpr uint32_t make_magic32(uint8_t a,  uint8_t b, uint8_t c)
	{
		return ((uint32_t)a << 16) | ((uint32_t)b << 8) | ((uint32_t)c);
	}

	static constexpr uint16_t VOICEDB_HEADER_SIZE = 43;
	static constexpr uint16_t PITCH_DATA_SECTION_SIZE = 15;
	static constexpr uint16_t STRING_TABLE_HEADER_SIZE = 7;
	static constexpr uint16_t TABLE_SECTION_SIZE = 5;
	static constexpr uint16_t DURATION_TABLE_ENTRY_SIZE = 18;
	static constexpr uint16_t PHONEME_UNIT_TABLE_ENTRY_SIZE = 7;
	static constexpr uint16_t PHONEME_TABLE_ENTRY_SIZE = 19;

	static constexpr uint32_t PITCH_DATA_MAGIC = make_magic32('P', 'T', 'C');
	static constexpr uint32_t STRING_TABLE_MAGIC = make_magic32('S', 'T', 'R');
	static constexpr uint32_t DURATION_TABLE_MAGIC = make_magic32('D', 'U', 'R');
	static constexpr uint32_t PHONEME_UNIT_TABLE_MAGIC = make_magic32('P', 'U', 'T');
	static constexpr uint32_t PHONEME_TABLE_MAGIC = make_magic32('P', 'H', 'O');

	std::shared_ptr<duration_model>
	createDurationModel(native_endian_buffer &aData);

	void read_phoneme_units(cainteoir::native_endian_buffer &data, std::vector<unit_t> &units);

	// LangDB Data Parser Helpers //////////////////////////////////////////////////////////

	static constexpr uint16_t LANGDB_HEADER_ID = 8; // magic + endianness
	static constexpr uint16_t LANGDB_HEADER_SIZE = 17;
	static constexpr uint16_t LETTER_TO_PHONEME_TABLE_SIZE = 6;
	static constexpr uint16_t LETTER_TO_PHONEME_TABLE_ENTRY_SIZE = 8;
	static constexpr uint16_t LEXICAL_REWRITE_RULES_TABLE_SIZE = 6;
	static constexpr uint16_t LEXICAL_REWRITE_RULES_TABLE_ENTRY_SIZE = 8;
	static constexpr uint16_t DICTIONARY_TABLE_SIZE = 5;
	static constexpr uint16_t DICTIONARY_TABLE_ENTRY_SIZE = 8;
	static constexpr uint16_t CLASSDEF_TABLE_SIZE = 6;
	static constexpr uint16_t CLASSDEF_TABLE_ENTRY_SIZE = 4;
	static constexpr uint16_t CONDRULE_TABLE_SIZE = 5;
	static constexpr uint16_t CONDRULE_TABLE_ENTRY_SIZE = 6;

	static constexpr uint32_t LETTER_TO_PHONEME_TABLE_MAGIC = make_magic32('L', '2', 'P');
	static constexpr uint32_t LEXICAL_REWRITE_RULE_TABLE_MAGIC = make_magic32('L', 'R', 'R');
	static constexpr uint32_t DICTIONARY_TABLE_MAGIC = make_magic32('D', 'I', 'C');
	static constexpr uint32_t CLASSDEF_TABLE_MAGIC = make_magic32('C', 'L', 'S');
	static constexpr uint32_t CONDRULE_TABLE_MAGIC = make_magic32('C', 'N', 'D');

	static constexpr uint8_t LANGDB_CONDRULE_SET_MASK = 0x80;
	static constexpr uint8_t LANGDB_CONDRULE_SET      = 0x00;
	static constexpr uint8_t LANGDB_CONDRULE_UNSET    = 0x80;

	static constexpr uint8_t LANGDB_CONDRULE_LOCALE = 1;

	// Voice Synthesizers //////////////////////////////////////////////////////////////////

	std::shared_ptr<voice>
	create_mbrola_voice(const std::shared_ptr<cainteoir::buffer> &aData,
	                    const rdf::graph &aMetadata,
	                    const rdf::uri &aVoice);
}}

#endif
