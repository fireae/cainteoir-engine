/* Espeak Text-to-Speech Engine.
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

#include <cainteoir/engines.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

struct speak_data
{
	tts::engine *engine;
	cainteoir::audio *audio;
	std::list<cainteoir::event> events;

	speak_data(tts::engine *aEngine, cainteoir::audio *aAudio, const std::list<cainteoir::event> &aEvents)
		: engine(aEngine)
		, audio(aAudio)
		, events(aEvents)
	{
	}
};

void * speak_tts_thread(void *data)
{
	speak_data *speak = (speak_data *)data;

	speak->audio->open();

	foreach_iter(event, speak->events)
	{
		if (event->type == cainteoir::text_event)
			speak->engine->speak(event->data.get(), speak->audio);
	}

	speak->audio->close();
	delete speak;
}

tts::engines::engines(rdf::graph &metadata)
{
	std::string uri;
	active = tts::create_espeak_engine(metadata, uri);
	enginelist[uri] = active;

	if (!active)
		throw std::runtime_error("no text-to-speech voices found.");
}

tts::engines::~engines()
{
	foreach_iter(engine, enginelist)
		delete engine->second;
}

int tts::engines::get_channels() const
{
	return active->get_channels();
}

int tts::engines::get_frequency() const
{
	return active->get_frequency();
}

cainteoir::audio_format tts::engines::get_audioformat() const
{
	return active->get_audioformat();
}

bool tts::engines::select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	engine *engine = NULL;
	std::string voice;

	foreach_iter(statement, rql::select(aMetadata, rql::subject, aVoice))
	{
		if (rql::predicate(*statement) == rdf::tts("name"))
			voice = rql::value(rql::object(*statement));
		else if (rql::predicate(*statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri *uri = rql::object(*statement);
			if (uri)
				engine = enginelist[ uri->str() ];
		}
	}

	if (engine && !voice.empty() && engine->select_voice(voice.c_str()))
	{
		active = engine;
		return true;
	}

	return false;
}

void tts::engines::speak(buffer *text, audio *out)
{
	active->speak(text, out);
}

void tts::engines::speak(const std::list<event> &events, audio *out)
{
	pthread_t threadId;
	int ret = pthread_create(&threadId, NULL, speak_tts_thread, (void *)new speak_data(active, out, events));
	pthread_join(threadId, NULL);
}
