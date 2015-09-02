/* Multi-type Object API.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_OBJECT_HPP
#define CAINTEOIR_ENGINE_OBJECT_HPP

#include "buffer.hpp"
#include "phoneme.hpp"

#include <map>

namespace cainteoir
{
	enum class object_type : uint16_t
	{
		null,
		boolean,
		integer,
		real,
		string,
		buffer,
		phoneme,
		range,
		dictionary,
	};

	struct object
	{
#if (INTPTR_MAX == INT64_MAX) // 64-bit
		typedef int64_t intval_t;
		typedef double  floatval_t;
#else // 32-bit
		typedef int32_t intval_t;
		typedef float   floatval_t;
#endif

		struct string_compare
		{
			bool operator()(const char *a, const char *b) const
			{
				return strcmp(a, b) < 0;
			}
		};

		typedef std::shared_ptr<cainteoir::buffer> buffer_t;
		typedef cainteoir::range<uint32_t> range_t;
		typedef std::shared_ptr<std::map<const char *, object, string_compare>> dictionary_t;

		object()
			: mType(object_type::null)
			, mIntVal(0)
		{
		}

		object(bool aValue)
			: mType(object_type::boolean)
			, mBoolVal(aValue)
		{
		}

		object(int32_t aValue)
			: mType(object_type::integer)
			, mIntVal(aValue)
		{
		}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
		object(int64_t aValue)
			: mType(object_type::integer)
			, mIntVal(aValue)
		{
		}
#endif

		object(float aValue)
			: mType(object_type::real)
			, mFloatVal(aValue)
		{
		}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
		object(double aValue)
			: mType(object_type::real)
			, mFloatVal(aValue)
		{
		}
#endif

		object(const char *aValue)
			: mType(object_type::string)
			, mStringVal(aValue)
		{
		}

		object(const std::shared_ptr<cainteoir::buffer> &aValue);

		object(const ipa::phoneme &aValue)
			: mType(object_type::phoneme)
			, mPhonemeVal(aValue)
		{
		}

		object(const cainteoir::range<uint32_t> &aValue);

		object(const object_type aType);

		object(const object &o);

		~object();

		object &operator=(const object &o);

		const object_type type() const { return mType; }

		bool is_null() const { return type() == object_type::null; }
		bool is_boolean() const { return type() == object_type::boolean; }
		bool is_integer() const { return type() == object_type::integer; }
		bool is_real() const { return type() == object_type::real; }
		bool is_number() const { return type() == object_type::integer || type() == object_type::real; }
		bool is_string() const { return type() == object_type::string; }
		bool is_buffer() const { return type() == object_type::buffer; }
		bool is_phoneme() const { return type() == object_type::phoneme; }
		bool is_range() const { return type() == object_type::range; }
		bool is_dictionary() const { return type() == object_type::dictionary; }

		bool boolean() const { return mBoolVal; }

		intval_t integer() const { return mIntVal; }

		floatval_t real() const { return mFloatVal; }

		floatval_t number() const { return is_integer() ? (floatval_t)mIntVal : mFloatVal; }

		const char *string() const { return mStringVal; }

		const buffer_t &buffer() const { return mBufferVal; }

		const ipa::phoneme &phoneme() const { return mPhonemeVal; }

		const range_t &range() const { return mRangeVal; }

		const object *get(const char *aKey) const;

		bool put(const char *aKey, const object &aValue);
	private:
		object_type mType;
		union
		{
			bool mBoolVal;
			intval_t mIntVal;
			floatval_t mFloatVal;
			const char *mStringVal;
			buffer_t mBufferVal;
			ipa::phoneme mPhonemeVal;
			range_t mRangeVal;
			dictionary_t mDictionaryVal;
		};

		void clear();

		void copy(const object &o);
	};
}

#endif
