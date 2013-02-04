/* Text Processing API.
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

#ifndef CAINTEOIR_ENGINE_TEXT_HPP
#define CAINTEOIR_ENGINE_TEXT_HPP

#include "buffer.hpp"
#include <ucd/ucd.h>

namespace cainteoir { namespace tts
{
	struct text_reader
	{
		enum token_type
		{
			error,
			word_uppercase,
			word_lowercase,
			word_capitalized,
			word_mixedcase,
			number,
			punctuation,
			symbol,
		};

		text_reader();

		cainteoir::buffer match() const { return cainteoir::buffer(mMatch, mMatchEnd); }

		token_type type() const { return mType; }

		ucd::script script() const { return mScript; }

		void set_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer);

		bool read();
	private:
		token_type mType;
		ucd::script mScript;

		char mMatch[512];
		char *mMatchEnd;

		const char *mStart;
		const char *mCurrent;
		const char *mLast;
		uint8_t mState;
	};
}}

#endif