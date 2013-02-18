/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>

#include <stdexcept>
#include <cstdio>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

static const char *token_name[] = {
	"error",
	"upper",
	"lower",
	"capital",
	"mixed",
	"script",
	"number",
	"punctuation",
	"comma",
	"semicolon",
	"colon",
	"ellipsis",
	"full-stop",
	"exclamation",
	"question",
	"symbol",
	"end-para",
};

template <typename Reader>
void generate_events(std::shared_ptr<cainteoir::document_reader> &reader)
{
	Reader text;
	while (reader->read())
	{
		text.next_item(*reader);
		while (text.read())
		{
			auto &event = text.event();
			switch (event.type)
			{
			case tts::word_uppercase:
			case tts::word_lowercase:
			case tts::word_capitalized:
			case tts::word_mixedcase:
			case tts::word_script:
				fprintf(stdout, ".%s.%-8s [%d..%d] %s\n",
				        ucd::get_script_string(event.script),
				        token_name[event.type],
				        event.range.begin(),
				        event.range.end(),
				        event.text->str().c_str());
				break;
			case tts::paragraph:
				fprintf(stdout, ".%-13s [%d..%d] \n",
				        token_name[event.type],
				        event.range.begin(),
				        event.range.end());
				break;
			default:
				fprintf(stdout, ".%-13s [%d..%d] %s\n",
				        token_name[event.type],
				        event.range.begin(),
				        event.range.end(),
				        event.text->str().c_str());
				break;
			}
		}
	}
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 2)
			throw std::runtime_error("no document specified");

		bool word_stream = !strcmp(argv[0], "wordstream");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[1], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[1]);
			return 0;
		}

		if (word_stream)
			generate_events<tts::word_stream>(reader);
		else
			generate_events<tts::text_reader>(reader);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
