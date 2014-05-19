/* Experimental interface to the MBROLA synthesizer.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#include <cainteoir/engines.hpp>
#include <stdexcept>

#include <unistd.h>
#include <fcntl.h>

namespace css = cainteoir::css;
namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace rdf = cainteoir::rdf;

#if defined(HAVE_MBROLA)

struct procstat_t
{
	enum status_t
	{
		running,  // R (running)
		idle,     // S (sleeping)   -- interruptible
		sleeping, // D (disk sleep) -- uninterruptible
		zombie,   // Z (zombie)
		stopped,  // T (traced/stopped on a signal)
		paging,   // W (paging)
	};

	procstat_t() : fd(-1) {}
	~procstat_t() { close(fd); }

	void open(pid_t pid);

	status_t stat() const;
private:
	int fd;
};

void procstat_t::open(pid_t pid)
{
	if (fd != -1) close(fd);

	char filename[256];
	snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
	fd = ::open(filename, O_RDONLY);
	if (fd == -1) throw std::runtime_error(strerror(errno));
}

procstat_t::status_t procstat_t::stat() const
{
	char buffer[20];
	if (lseek(fd, 0, SEEK_SET) != 0)
		throw std::runtime_error(strerror(errno));
	if (read(fd, buffer, sizeof(buffer)) != sizeof(buffer))
		throw std::runtime_error(strerror(errno));

	char *s = (char *)memchr(buffer, ')', sizeof(buffer));
	if (!s || (s - buffer) >= sizeof(buffer) - 1 || s[1] != ' ')
		throw std::runtime_error("unable to determine the state of the MBROLA process");
	switch (s[2])
	{
	case 'R': return running;
	case 'S': return idle;
	case 'D': return sleeping;
	case 'Z': return zombie;
	case 'T': return stopped;
	case 'W': return paging;
	default:
		throw std::runtime_error("unable to determine the state of the MBROLA process");
	}
}

struct pipe_t
{
	enum fd_t
	{
		read_fd  = 0,
		write_fd = 1,
	};

	pipe_t()
		: fds{ -1, -1 }
	{
		if (pipe(fds) == -1)
			throw std::runtime_error(strerror(errno));
	}

	~pipe_t()
	{
		::close(fds[read_fd]);
		::close(fds[write_fd]);
	}

	void dup(fd_t type, int fd)
	{
		if (dup2(fds[type], fd) == -1)
			throw std::runtime_error(strerror(errno));
		close(type == read_fd ? write_fd : read_fd);
	}

	void close(fd_t type)
	{
		::close(fds[type]);
		fds[type] = -1;
	}

	ssize_t read(void *buf, size_t count, const procstat_t &proc);

	FILE *open(const char *mode)
	{
		FILE *ret = fdopen(fds[(mode[0] == 'r') ? read_fd : write_fd], mode);
		if (!ret) throw std::runtime_error(strerror(errno));
		fds[(mode[0] == 'r') ? read_fd : write_fd] = -1;
		return ret;
	}

	void set_flags(fd_t type, int flags)
	{
		fcntl(fds[type], F_SETFL, fcntl(fds[type], F_GETFL) | flags);
	}

	void clear_flags(fd_t type, int flags)
	{
		fcntl(fds[type], F_SETFL, fcntl(fds[type], F_GETFL) & ~flags);
	}
private:
	int fds[2];
};

ssize_t pipe_t::read(void *buf, size_t count, const procstat_t &proc)
{
	for (;;)
	{
		ssize_t ret = ::read(fds[read_fd], buf, count);
		if (ret < 0) switch (errno)
		{
		case EINTR:
			continue; // normal... try again
		case EAGAIN:
			{
				if (proc.stat() == procstat_t::idle)
					return 0; // idle... no data

				fd_set rset;
				FD_ZERO(&rset);
				FD_SET(fds[read_fd], &rset);
				timeval timeout = { 60, 0 }; // 1 min
				int ret = select(fds[read_fd] + 1, &rset, nullptr, nullptr, &timeout);
				if (ret < 0) // error
					throw std::runtime_error(strerror(errno));
				if (ret == 0)
					return 0; // timeout... no data
			}
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
		else
			return ret;
	}
}

struct mbrola_synthesizer : public tts::synthesizer
{
	mbrola_synthesizer(const char *database, std::shared_ptr<tts::prosody_writer> aWriter);

	~mbrola_synthesizer();

	/** @name audio_info */
	//@{

	int channels() const { return 1; }

	int frequency() const { return sample_rate; }

	const rdf::uri &format() const { return sample_format; }

	//@}
	/** @name tts::prosody_writer */
	//@{

	void reset(FILE *aOutput) {}

	bool write(const tts::prosody &aProsody);

	//@}
	/** @name tts::synthesizer */
	//@{

	bool read(cainteoir::audio *out);

	//@}
private:
	void flush();

	enum state_t
	{
		need_data,
		have_data,
	};

	pid_t pid;
	procstat_t proc;
	FILE *pho;
	pipe_t audio;
	state_t state;

	int sample_rate;
	rdf::uri sample_format;

	std::shared_ptr<tts::prosody_writer> writer;
};

mbrola_synthesizer::mbrola_synthesizer(const char *aDatabase, std::shared_ptr<tts::prosody_writer> aWriter)
	: pho(nullptr)
	, state(need_data)
	, sample_rate(0)
	, sample_format(rdf::tts("s16le"))
	, writer(aWriter)
{
	pipe_t input;
	pipe_t error;

	pid = fork();
	if (pid == -1) throw std::runtime_error(strerror(errno));
	if (pid == 0)
	{
		input.dup(pipe_t::read_fd, STDIN_FILENO);
		audio.dup(pipe_t::write_fd, STDOUT_FILENO);
		error.dup(pipe_t::write_fd, STDERR_FILENO);

		execlp("mbrola", "mbrola",
		       "-e",      // ignore fatal errors on unknown diphones
		       aDatabase, // voice file database
		       "-",       // pho file input (stdin)
		       "-.wav",   // audio output (stdout)
                       nullptr);
		_exit(1);
	}

	proc.open(pid);

	input.close(pipe_t::read_fd);
	audio.close(pipe_t::write_fd);
	error.close(pipe_t::write_fd);

	input.set_flags(pipe_t::write_fd, O_NONBLOCK);
	audio.set_flags(pipe_t::read_fd,  O_NONBLOCK);
	error.set_flags(pipe_t::read_fd,  O_NONBLOCK);

	pho = input.open("w");
	writer->reset(pho);

	flush();
	uint8_t header[44];
	if (audio.read(header, sizeof(header), proc) != sizeof(header))
		throw std::runtime_error("MBROLA did not return a WAV header.");

	if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVEfmt ", 8) != 0)
		throw std::runtime_error("MBROLA did not return a WAV header.");

	sample_rate = header[24] + (header[25] << 8) + (header[26] << 16) + (header[27] << 24);
}

mbrola_synthesizer::~mbrola_synthesizer()
{
	fclose(pho);
}

bool mbrola_synthesizer::write(const tts::prosody &aProsody)
{
	if (!writer->write(aProsody))
		return false;

	state = have_data;
	return true;
}

bool mbrola_synthesizer::read(cainteoir::audio *out)
{
	if (state != have_data) return false;
	flush();

	short data[1024];
	ssize_t read;
	while ((read = audio.read(data, sizeof(data), proc)) > 0)
		out->write((const char *)data, read);

	state = need_data;
	return true;
}

void mbrola_synthesizer::flush()
{
	fputs("#\n", pho);
	fflush(pho);
}

std::shared_ptr<tts::synthesizer> create_mbrola_voice(const char *voice)
{
	if (!voice || strlen(voice) != 3)
		return {}; // invalid voice name

	char database[256];
	snprintf(database, sizeof(database), MBROLA_DIR "/%s/%s", voice, voice);

	char phonemeset[11];
	snprintf(phonemeset, sizeof(phonemeset), "mbrola/%s", voice);

	auto phonemes = tts::createPhonemeWriter(phonemeset);
	if (!phonemes) return {};

	return std::make_shared<mbrola_synthesizer>(database, tts::createPhoWriter(phonemes));
}

#else

std::shared_ptr<tts::synthesizer> create_mbrola_voice(const char *voice)
{
	return {};
}

#endif

int main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
		{
			fprintf(stderr, "usage: mbrola VOICE_NAME\n");
			return 0;
		}

		auto voice = create_mbrola_voice(argv[1]);
		if (!voice) throw std::runtime_error("cannot find the specified MBROLA voice");

		fprintf(stdout, "channels    : %d\n", voice->channels());
		fprintf(stdout, "format      : %s\n", voice->format().str().c_str());
		fprintf(stdout, "sample rate : %d\n", voice->frequency());

		rdf::graph metadata;
		rdf::uri doc;
		auto out = cainteoir::open_audio_device(nullptr, metadata, doc,
			voice->format(),
			voice->channels(),
			voice->frequency());
		out->open();

		voice->write({ ipa::velar | ipa::plosive, ipa::unspecified, // k
		               { 80, css::time::milliseconds },
		               {}});
		voice->write({ ipa::semi_high | ipa::front | ipa::vowel, ipa::unspecified, // I
		               { 80, css::time::milliseconds },
		               {}});
		voice->write({ ipa::alveolar | ipa::plosive, ipa::unspecified, // t
		               { 80, css::time::milliseconds },
		               {}});

		voice->read(out.get());

		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
