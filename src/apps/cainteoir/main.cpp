/* Cainteoir Command-Line Application.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/tts_engine.hpp>
#include <cstring>
#include <cstdio>
#include <memory>

int main(int argc, char ** argv)
{
	try
	{
		const char *text = "Hello World!";
		std::auto_ptr<cainteoir::buffer> text_buffer;

		if (argc > 1)
			text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[1]));
		else
			text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::buffer(text, text+strlen(text)+1));

		std::auto_ptr<cainteoir::tts_engine> tts = cainteoir::create_espeak_engine();
		tts->speak(text_buffer.get());
	}
	catch (std::exception &e)
	{
		printf("error: %s\n", e.what());
	}

	return 0;
}
