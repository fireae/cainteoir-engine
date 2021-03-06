/* Test for the trie data structure.
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

#include <cainteoir/trie.hpp>
#include <cainteoir/buffer.hpp>

#include "tester.hpp"

#include <vector>

REGISTER_TESTSUITE("trie");

TEST_CASE("trie node (key=char): root")
{
	cainteoir::trie_node<int> n('\0');
	assert(n.c == '\0');
	assert(n.item == 0);
	assert(n.children.empty());
}

TEST_CASE("trie node (key=char): insert node")
{
	cainteoir::trie_node<int> n('\0');

	assert(n.get('d') == nullptr);
	assert(n.children.size() == 0);

	assert(n.add('d') != nullptr);
	assert(n.children.size() == 1);
	assert(n.add('d') == n.add('d'));
	assert(n.children.size() == 1);
	assert(n.get('d') == n.add('d'));
	assert(n.children.size() == 1);

	assert(n.add('e') != nullptr);
	assert(n.get('d') != n.get('e'));

	assert(n.get('d')->c == 'd');
	assert(n.get('e')->c == 'e');
}

TEST_CASE("trie (key=char): cainteoir::buffer insertion")
{
	static const std::initializer_list<std::pair<cainteoir::buffer, int>> words =
	{
		{ "test", 1 },
		{ "tear", 2 },
		{ "tome", 3 },
		{ "boast", 4 },
		{ "view", 5 },
		{ "even", 6 },
		{ "viewing", 7 },
	};

	cainteoir::trie<int> n;
	for (const auto &word : words)
		n.insert(word.first, word.second);

	for (const auto &word : words)
		assert(n.lookup(word.first) == word.second);

	assert(n.lookup("team") == 0);
	assert(n.lookup("nosuchword") == 0);
}

TEST_CASE("trie (key=char): std::string insertion")
{
	static const std::initializer_list<std::pair<std::string, int>> words =
	{
		{ "test", 1 },
		{ "tear", 2 },
		{ "tome", 3 },
		{ "boast", 4 },
		{ "view", 5 },
		{ "even", 6 },
		{ "viewing", 7 },
	};

	cainteoir::trie<int> n;
	for (const auto &word : words)
		n.insert(word.first, word.second);

	for (const auto &word : words)
		assert(n.lookup(word.first) == word.second);

	assert(n.lookup("team") == 0);
	assert(n.lookup("nosuchword") == 0);
}

TEST_CASE("trie node (key=long): root")
{
	cainteoir::trie_node<int, long> n(-1);
	assert(n.c == -1);
	assert(n.item == 0);
	assert(n.children.empty());
}

TEST_CASE("trie node (key=long): insert node")
{
	cainteoir::trie_node<int, long> n(-1);

	assert(n.get(4) == nullptr);
	assert(n.children.size() == 0);

	assert(n.add(4) != nullptr);
	assert(n.children.size() == 1);
	assert(n.add(4) == n.add(4));
	assert(n.children.size() == 1);
	assert(n.get(4) == n.add(4));
	assert(n.children.size() == 1);

	assert(n.add(5) != nullptr);
	assert(n.get(4) != n.get(5));

	assert(n.get(4)->c == 4);
	assert(n.get(5)->c == 5);
}

TEST_CASE("trie (key=long): std::vector insertion")
{
	static const std::initializer_list<std::pair<std::vector<long>, int>> words =
	{
		{ { 1, 2, 3 }, 1 },
		{ { 7, 4, 6 }, 2 },
		{ { 8, 7, 6, 5, 4 }, 3 },
		{ { 7, 4,  9 }, 4 },
		{ { 2, 2 }, 5 },
	};

	cainteoir::trie<int, long> n;
	for (const auto &word : words)
		n.insert(word.first, word.second);

	for (const auto &word : words)
		assert(n.lookup(word.first) == word.second);

	assert(n.lookup(std::vector<long>({ 7, 4 })) == 0);
	assert(n.lookup(std::vector<long>({ 6, 2, 6 })) == 0);
}
