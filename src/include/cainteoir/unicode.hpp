/* Unicode Support.
 *
 * Copyright (C) 2012-2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_UNICODE_HPP
#define CAINTEOIR_ENGINE_UNICODE_HPP

#include "buffer.hpp"

namespace cainteoir { namespace utf8
{
	char *write(char *out, uint32_t c);

	const char *read(const char *in, uint32_t &c);

	const char *next(const char *c);

	const char *prev(const char *c);

	int32_t codepoints(const char *first, const char *last);
}}

#endif
