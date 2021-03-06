/* Compressed Stream (GZip)
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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
#include "i18n.h"

#include <cainteoir/buffer.hpp>
#include <zlib.h>
#include <stdexcept>

static std::shared_ptr<cainteoir::buffer> inflateBuffer(const cainteoir::buffer &compressed, uint32_t uncompressed, int window)
{
	std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(uncompressed);

	z_stream strm = {0};
	int ret = inflateInit2(&strm, window);
	if (ret != Z_OK)
		goto err;

	strm.avail_in = compressed.size();
	strm.next_in = (Bytef *)compressed.begin();

	strm.avail_out = uncompressed;
	strm.next_out = (Bytef *)data->begin();

	ret = inflate(&strm, Z_SYNC_FLUSH);
	(void)inflateEnd(&strm);

	if (ret != Z_STREAM_END)
		goto err;

	return data;

err:
	switch (ret)
	{
	case Z_STREAM_ERROR:
		throw std::runtime_error(i18n("decompression failed (invalid compression level)"));
	case Z_DATA_ERROR:
		throw std::runtime_error(i18n("decompression failed (invalid or incomplete deflate data)"));
	case Z_MEM_ERROR:
		throw std::runtime_error(i18n("decompression failed (out of memory)"));
	case Z_VERSION_ERROR:
		throw std::runtime_error(i18n("decompression failed (zlib version mismatch)"));
	default:
		throw std::runtime_error(i18n("decompression failed (unspecified error)"));
	}
}

std::shared_ptr<cainteoir::buffer> cainteoir::inflate_zlib(const cainteoir::buffer &data, uint32_t size)
{
	return inflateBuffer(data, size, -MAX_WBITS);
}

std::shared_ptr<cainteoir::buffer> cainteoir::inflate_gzip(const cainteoir::buffer &data, uint32_t size)
{
	size = *((uint32_t *)(data.end() - 4));
	return inflateBuffer(data, size, 16 + MAX_WBITS);
}
