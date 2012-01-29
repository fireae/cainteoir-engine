/* Audio API.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_AUDIO_HPP
#define CAINTEOIR_ENGINE_AUDIO_HPP

#include "metadata.hpp"
#include <stdint.h>
#include <tr1/memory>
#include <string>
#include <list>

namespace cainteoir
{
	struct audio
	{
		virtual ~audio() {}

		virtual void open() = 0;

		virtual void close() = 0;

		virtual uint32_t write(const char *data, uint32_t len) = 0;
	};

	/** @name  Metadata
	  * @brief Converters to map RDF metadata to metadata used by various audio formats.
	  */
	//@{

	/** @brief A vorbis comment.
	  * @see   http://www.xiph.org/vorbis/doc/v-comment.html
	  */
	class vorbis_comment
	{
	public:
		const std::string label; /**< @brief The name of the comment, e.g. TITLE. */
		const std::string value; /**< @brief The content of the comment, e.g. "Moonlight Sonata". */

		vorbis_comment(const std::string &aLabel, const std::string &aValue)
			: label(aLabel)
			, value(aValue)
		{
		}
	};

	/** @brief Convert an RDF model to a set of Vorbis Comments.
	  * @see   http://www.xiph.org/vorbis/doc/v-comment.html
	  * @see   http://wiki.xiph.org/Metadata
	  *
	  * @param aMetadata The RDF metadata used to create the vorbis comments.
	  * @param aDocument The URI of the document in the RDF model to get the data for.
	  * @return          The vorbis comments in (label, value) form.
	  */
	std::list<vorbis_comment> vorbis_comments(const rdf::graph &aMetadata, const rdf::uri &aDocument);

	//@}

	void supportedAudioFormats(rdf::graph &metadata);

	std::tr1::shared_ptr<audio>
	create_audio_file(
		const char *filename,
		const char *type,
		float quality,
		const rdf::graph &aDocMetadata,
		const rdf::uri &aDocument,
		const rdf::graph &aVoiceMetadata,
		const rdf::uri &aVoice);

	std::tr1::shared_ptr<audio>
	open_audio_device(
		const char *device,
		const char *type,
		float quality,
		const rdf::graph &aDocMetadata,
		const rdf::uri &aDocument,
		const rdf::graph &aVoiceMetadata,
		const rdf::uri &aVoice);
}

#endif
