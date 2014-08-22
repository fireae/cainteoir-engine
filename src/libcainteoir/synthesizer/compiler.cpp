/* Voice Database File Builder.
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
#include "compatibility.hpp"

#include <cainteoir/synthesizer.hpp>
#include "../cainteoir_file_reader.hpp"

namespace tts = cainteoir::tts;

struct binary_file_writer
{
	binary_file_writer(FILE *aOutput)
		: mOutput(aOutput)
		, mOffset(0)
	{
	}

	void begin_section(const char *magic, uint16_t section_size);
	void end_section();

	void u8(uint8_t u)   { fputc(u, mOutput); }
	void u16(uint16_t u) { fwrite(&u, sizeof(u), 1, mOutput); }
	void u32(uint32_t u) { fwrite(&u, sizeof(u), 1, mOutput); }

	void f8_8(float f);

	void  str(const cainteoir::buffer &data);
	void pstr(const cainteoir::buffer &data);
private:
	FILE *mOutput;
	uint16_t mOffset;
	std::list<cainteoir::buffer> mStrings;
};

void binary_file_writer::begin_section(const char *magic, uint16_t section_size)
{
	mOffset += section_size;
	fputs(magic, mOutput);
}

void binary_file_writer::end_section()
{
	for (const auto &s : mStrings)
		str(s);
	mStrings.clear();
}

void binary_file_writer::f8_8(float f)
{
	u8((int8_t)f);
	u8(f * 256);
}

void binary_file_writer::str(const cainteoir::buffer &data)
{
	fwrite(data.begin(), data.size(), 1, mOutput);
	fputc('\0', mOutput);
}

void binary_file_writer::pstr(const cainteoir::buffer &data)
{
	u16(mOffset);
	mOffset += data.size() + 1;
	mStrings.push_back(data);
}

void
tts::compile_voice(const char *aFileName, FILE *aOutput)
{
	if (!aOutput) return;

	cainteoir::buffer rdfns{ nullptr, nullptr };
	cainteoir::buffer id{ nullptr, nullptr };
	cainteoir::buffer name{ nullptr, nullptr };
	cainteoir::buffer synthesizer{ nullptr, nullptr };
	cainteoir::buffer author{ nullptr, nullptr };
	cainteoir::buffer phonemeset{ nullptr, nullptr };
	uint8_t gender = 0;
	float volume_scale = 1.0;
	uint16_t frequency = 0;
	uint8_t channels = 0;
	cainteoir::buffer sample_format{ nullptr, nullptr };

	auto reader = cainteoir_file_reader(cainteoir::path(aFileName));
	while (reader.read())
	{
		if (reader.type() == cainteoir_file_reader::directive)
		{
			if (reader.match().compare(".rdfns") == 0)
			{
				reader.read();
				rdfns = reader.match();
			}
			else if (reader.match().compare(".id") == 0)
			{
				reader.read();
				id = reader.match();
			}
			else if (reader.match().compare(".name") == 0)
			{
				reader.read();
				name = reader.match();
			}
			else if (reader.match().compare(".synthesizer") == 0)
			{
				reader.read();
				synthesizer = reader.match();
			}
			else if (reader.match().compare(".voice-author") == 0)
			{
				reader.read();
				author = reader.match();
			}
			else if (reader.match().compare(".phonemeset") == 0)
			{
				reader.read();
				phonemeset = reader.match();
			}
			else if (reader.match().compare(".gender") == 0)
			{
				reader.read();
				if (reader.match().compare("male") == 0)
					gender = 'M';
				else if (reader.match().compare("female") == 0)
					gender = 'F';
				else
					throw std::runtime_error("unsupported gender value");
			}
			else if (reader.match().compare(".volume-scale") == 0)
			{
				reader.read();
				volume_scale = strtof(reader.match().begin(), nullptr);
			}
			else if (reader.match().compare(".frequency") == 0)
			{
				reader.read();
				frequency = strtol(reader.match().begin(), nullptr, 10);
			}
			else if (reader.match().compare(".channels") == 0)
			{
				reader.read();
				channels = strtol(reader.match().begin(), nullptr, 10);
			}
			else if (reader.match().compare(".sample-format") == 0)
			{
				reader.read();
				sample_format = reader.match();
			}
		}
	}

	binary_file_writer out(aOutput);

	// Voice Database Header
	out.begin_section("VOICEDB", 29);
	out.u16(0x3031); // endianness
	out.pstr(rdfns);
	out.pstr(id);
	out.pstr(name);
	out.pstr(synthesizer);
	out.pstr(author);
	out.pstr(phonemeset);
	out.u8(gender);
	out.f8_8(volume_scale);
	out.u16(frequency);
	out.u8(channels);
	out.pstr(sample_format);
	out.end_section();
}
