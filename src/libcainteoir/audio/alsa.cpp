/* ALSA Support.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/audio.hpp>

namespace rdf = cainteoir::rdf;

#ifdef HAVE_ALSA

#include <stdexcept>
#include <alsa/asoundlib.h>

#define check(x) { int err = x; if (err < 0) throw std::runtime_error(std::string(i18n("alsa: ")) + snd_strerror(err)); }

struct format_info_t
{
	const char *format;
	int sample_size;
	snd_pcm_format_t pcm_format;
};

static const format_info_t format_info[] =
{
	{ "s8",        1, SND_PCM_FORMAT_S8 },
	{ "u8",        1, SND_PCM_FORMAT_U8 },
	{ "s16le",     2, SND_PCM_FORMAT_S16_LE },
	{ "s16be",     2, SND_PCM_FORMAT_S16_BE },
	{ "u16le",     2, SND_PCM_FORMAT_U16_LE },
	{ "u16be",     2, SND_PCM_FORMAT_U16_BE },
	{ "s32le",     4, SND_PCM_FORMAT_S32_LE },
	{ "s32be",     4, SND_PCM_FORMAT_S32_BE },
	{ "u32le",     4, SND_PCM_FORMAT_U32_LE },
	{ "u32be",     4, SND_PCM_FORMAT_U32_BE },
	{ "float32le", 4, SND_PCM_FORMAT_FLOAT_LE },
	{ "float32be", 4, SND_PCM_FORMAT_FLOAT_BE },
	{ "float64le", 8, SND_PCM_FORMAT_FLOAT64_LE },
	{ "float64be", 8, SND_PCM_FORMAT_FLOAT64_BE },
};

class alsa_audio : public cainteoir::audio
{
	snd_pcm_t *mHandle;
	int mSampleSize;
	snd_pcm_format_t mFormat;
	unsigned int mRate;
	int mChannels;
	const char *mDevice;
	rdf::uri mFormatUri;
public:
	alsa_audio(const char *device, const rdf::uri &format, int channels, int frequency)
		: mHandle(nullptr)
		, mRate(frequency)
		, mChannels(channels)
		, mDevice(device ? device : "default")
		, mFormatUri(format)
	{
		if (format.ns == rdf::tts.href) for (const auto &info : format_info)
		{
			if (format.ref == info.format)
			{
				mFormat = info.pcm_format;
				mSampleSize = info.sample_size;
				return;
			}
		}
		throw std::runtime_error(i18n("unsupported audio format."));
	}

	~alsa_audio()
	{
		close();
	}

	void open()
	{
		snd_pcm_hw_params_t *params = nullptr;
		snd_pcm_hw_params_alloca(&params);
		check(snd_pcm_open(&mHandle, mDevice, SND_PCM_STREAM_PLAYBACK, 0));
		check(snd_pcm_hw_params_any(mHandle, params));
		check(snd_pcm_hw_params_set_access(mHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED));
		check(snd_pcm_hw_params_set_format(mHandle, params, mFormat));
		check(snd_pcm_hw_params_set_rate_near(mHandle, params, &mRate, 0));
		check(snd_pcm_hw_params_set_channels(mHandle, params, mChannels));
		check(snd_pcm_hw_params(mHandle, params));
		check(snd_pcm_prepare(mHandle));
	}

	void close()
	{
		if (mHandle)
		{
			snd_pcm_drop(mHandle);
			snd_pcm_close(mHandle);
			mHandle = nullptr;
		}
	}

	uint32_t write(const char *data, uint32_t len)
	{
		int ret = snd_pcm_writei(mHandle, data, len / mSampleSize);
		if (ret < 0)
		{
			snd_pcm_prepare(mHandle);
			fprintf(stderr, "alsa: buffer underrun\n");
			return 0;
		}
		return ret;
	}

	int channels() const { return mChannels; }

	int frequency() const { return mRate; }

	const rdf::uri &format() const { return mFormatUri; }
};

std::shared_ptr<cainteoir::audio>
create_alsa_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	return std::make_shared<alsa_audio>(device, format, channels, frequency);
}

#else

std::shared_ptr<cainteoir::audio>
create_alsa_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	return {};
}

#endif
