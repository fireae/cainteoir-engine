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

#include <cainteoir/tts_engine.hpp>
#include <espeak/speak_lib.h>

class espeak_engine : public cainteoir::tts_engine
{
	int m_frequency;
public:
	espeak_engine()
	{
		m_frequency = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 0, NULL, 0);
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::audio_properties */
	//@{

	int frequency() const
	{
		return m_frequency;
	}

	int channels() const
	{
		return cainteoir::mono;
	}

	cainteoir::audio_format format() const
	{
		return cainteoir::pcm_s16;
	}

	//@}
	/** @name cainteoir::metadata */
	//@{

	std::string get_metadata(const char *uri) const
	{
		if (!strcmp(uri, cainteoir::dc::title) || !strcmp(uri, cainteoir::dcterms::title))
			return "espeak";
		return std::string();
	}

	//@}
	/** @name cainteoir::tts_engine */
	//@{

	bool set_voice_by_name(const char *name)
	{
		if (!name) name = "default";
		return espeak_SetVoiceByName(name) == EE_OK;
	}

	void speak(cainteoir::buffer *text)
	{
		espeak_Synth(text->begin(), text->size(), 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, NULL, NULL);
		espeak_Synchronize();
	}

	//@}
};

std::auto_ptr<cainteoir::tts_engine> cainteoir::create_espeak_engine()
{
	return std::auto_ptr<cainteoir::tts_engine>(new espeak_engine());
}

