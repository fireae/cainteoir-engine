/* A voice synthesizer.
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
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/audio.hpp>
#include <cstdint>
#include <cstdio>
#include <cmath>

namespace rdf = cainteoir::rdf;

int main(int argc, char **argv)
{
	try
	{
		const char *device_name = nullptr;

		uint16_t sample_rate = 44100;
		float frequency = 440.0; // A4
		float duration = 1.0; // 1 second
		float amplitude = 4.0;

		const option_group general_options = { nullptr, {
			{ 'D', "device", device_name, "DEVICE",
			  i18n("Use DEVICE for audio output (ALSA/pulseaudio device name)") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("synthesizer [OPTION..]"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		rdf::graph metadata;
		rdf::uri doc;
		auto out = cainteoir::open_audio_device(device_name, metadata, doc, rdf::tts("float32le"), 1, sample_rate);
		out->open();

		float sample_duration = 1.0 / sample_rate;
		float t = 0.0;
		while (true)
		{
			float a = sin(2 * M_PI * frequency * t);
			a *= amplitude;
			out->write((const char *)&a, sizeof(a));

			if (t > duration) break;
			t += sample_duration;
		}

		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}
	return 0;
}
