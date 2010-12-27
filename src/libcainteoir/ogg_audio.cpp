/* Ogg+Vorbis Audio File.
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

#include "config.h"
#include <cainteoir/audio.hpp>

#if defined(HAVE_VORBISENC)

#include <vorbis/vorbisenc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

class ogg_audio : public cainteoir::audio
{
	FILE *m_file;

	ogg_stream_state os;
	ogg_page og;
	ogg_packet op;

	vorbis_info vi;
	vorbis_comment vc;
	vorbis_dsp_state vd;
	vorbis_block vb;

	void write_ogg_data(long i)
	{
		vorbis_analysis_wrote(&vd, i);

		while (vorbis_analysis_blockout(&vd, &vb) == 1)
		{
			vorbis_analysis(&vb, NULL);
			vorbis_bitrate_addblock(&vb);

			while (vorbis_bitrate_flushpacket(&vd, &op))
			{
				ogg_stream_packetin(&os, &op);

				while (!ogg_page_eos(&og))
				{
					int result = ogg_stream_pageout(&os, &og);
					if (result == 0)
						break;

					fwrite(og.header, 1, og.header_len, m_file);
					fwrite(og.body,   1, og.body_len,   m_file);
				}
			}
		}
	}
public:
	ogg_audio(FILE *f, cainteoir::audio_format format, int channels, int frequency, float quality) : m_file(f)
	{
		vorbis_info_init(&vi);
		vorbis_encode_init_vbr(&vi, channels, frequency, quality);

		vorbis_comment_init(&vc);
		vorbis_comment_add_tag(&vc, "ENCODER", "Cainteoir Text-to-Speech");

		vorbis_analysis_init(&vd, &vi);
		vorbis_block_init(&vd, &vb);

		srand(time(NULL));
		ogg_stream_init(&os, rand());
	}

	~ogg_audio()
	{
		close();
	}

	void open()
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
		ogg_stream_packetin(&os, &header);
		ogg_stream_packetin(&os, &header_comm);
		ogg_stream_packetin(&os, &header_code);

		while (true)
		{
			int result = ogg_stream_flush(&os, &og);
			if (result == 0)
				break;

			fwrite(og.header, 1, og.header_len, m_file);
			fwrite(og.body,   1, og.body_len,   m_file);
		}
	}

	void close()
	{
		if (!m_file)
			return;

		write_ogg_data(0);

		ogg_stream_clear(&os);
		vorbis_block_clear(&vb);
		vorbis_dsp_clear(&vd);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);

		if (m_file == stdout)
			fclose(m_file);
		m_file = NULL;
	}

	uint32_t write(const char *data, uint32_t len)
	{
		if (len == 0)
			return 0;

		float **buffer = vorbis_analysis_buffer(&vd, len);

		long i;
		for (i = 0; i < len/2; ++i)
			buffer[0][i] = ((data[i*2+1]<<8)|(0x00ff&(int)data[i*2]))/32768.f;

		write_ogg_data(i);

		return len;
	}
};

std::auto_ptr<cainteoir::audio> cainteoir::create_ogg_file(const char *filename, cainteoir::audio_format format, int channels, int frequency, float quality)
{
	FILE *file = filename ? fopen(filename, "wb") : stdout;
	return std::auto_ptr<cainteoir::audio>(new ogg_audio(file, format, channels, frequency, quality));
}

#else

std::auto_ptr<cainteoir::audio> cainteoir::create_ogg_file(const char *filename, cainteoir::audio_format format, int channels, int frequency, float quality)
{
	return std::auto_ptr<cainteoir::audio>();
}

#endif
