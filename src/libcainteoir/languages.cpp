/* BCP 47 Locale Support.
 *
 * Copyright (C) 2010-2015 Reece H. Dunn
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

#include <cainteoir/locale.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/path.hpp>
#include <algorithm>

namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;
namespace lang = cainteoir::language;

static std::string to_upper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

static std::string to_lower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

static std::string capitalize(std::string s)
{
	int len = s.length();
	if (len > 0)
	{
		s[0] = toupper(s[0]);
		std::transform(s.begin()+1, s.end(), s.begin()+1, ::tolower);
	}
	return s;
}

struct LanguageData
{
	LanguageData();

	std::map<std::string, std::string> subtags;
	std::map<std::string, lang::tag> extlangs;
};

LanguageData::LanguageData()
{
	rdf::graph data;
	try
	{
		auto path = cainteoir::get_data_path() / "languages.rdf.gz";
		cainteoir::createDocumentReader(path, data, std::string());
	}
	catch (const std::exception & e)
	{
		printf("error: %s\n", e.what());
	}

	for (auto &language : rql::select(data, rql::predicate == rdf::rdf("type")))
	{
		rql::results statements = rql::select(data, rql::subject == rql::subject(language));
		auto id     = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("code"));
		auto name   = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("label"));
		auto prefix = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("prefix"));

		subtags[id] = name;
		if (rql::object(language) == rdf::iana("ExtLang"))
			extlangs.insert({ id, { prefix, id }});
	}
}

static LanguageData *language_data()
{
	static std::shared_ptr<LanguageData> data;
	if (!data.get())
		data = std::make_shared<LanguageData>();
	return data.get();
}

static std::string localize_subtag(const char *iso_codes, const std::string &id)
{
	auto data = language_data();
	auto entry = data->subtags.find(id);
	if (entry == data->subtags.end())
		return id;
#ifdef ENABLE_NLS
	return dgettext(iso_codes, entry->second.c_str());
#else
	return entry->second;
#endif
}

static const std::initializer_list<std::pair<std::string, const lang::tag>> alias_tags = {
	{ "art-lojban",  { "jbo" } },
	{ "be@latin",    { "be", "", "Latn" } },
	{ "ca@valencia", { "ca", "", "", "", "valencia" } },
	{ "cel-gaulish", { "cel-gaulish" } }, // parent=cel, children=[xtg, xcg, xlp, xga]
	{ "en-gb-oed",   { "en", "", "", "GB" } },
	{ "en@boldquot", { "en" } },
	{ "en@quot",     { "en" } },
	{ "en@shaw",     { "en" } },
	{ "i-ami",       { "ami" } },
	{ "i-bnn",       { "bnn" } },
	{ "i-default",   { "und" } },
	{ "i-enochian",  { "i-enochian" } }, // no corresponding preferred tag
	{ "i-hak",       { "hak" } },
	{ "i-klingon",   { "tlh" } },
	{ "i-lux",       { "lb" } },
	{ "i-mingo",     { "i-mingo" } }, // no corresponding preferred tag
	{ "i-navajo",    { "nv" } },
	{ "i-pwn",       { "pwn" } },
	{ "i-tao",       { "tao" } },
	{ "i-tay",       { "tay" } },
	{ "i-tsu",       { "tsu" } },
	{ "no-bok",      { "nb" } },
	{ "no-nyn",      { "nn" } },
	{ "sgn-be-fr",   { "sfb" } },
	{ "sgn-be-nl",   { "vgt" } },
	{ "sgn-ch-de",   { "sgg" } },
	{ "sr@ije",      { "sr" } },
	{ "sr@latin",    { "sr", "", "Latn" } },
	{ "sr@latn",     { "sr", "", "Latn" } },
	{ "uz@cyrillic", { "uz", "", "Cyrl" } },
	{ "zh-guoyu",    { "zh", "cmn" } },
	{ "zh-hakka",    { "zh", "hak" } },
	{ "zh-min",      { "zh", "nan" } },
	{ "zh-min-nan",  { "zh", "nan" } },
	{ "zh-xiang",    { "zh", "hsn" } },
};

static const lang::tag *lookup_alias(std::string lang)
{
	lang = to_lower(lang);

	int begin = 0;
	int end = alias_tags.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = lang.compare((alias_tags.begin() + pos)->first);
		if (comp == 0)
			return &(alias_tags.begin() + pos)->second;
		else if (comp > 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return nullptr;
}

static const lang::tag *lookup_extlang(std::string lang)
{
	auto data = language_data();
	auto entry = data->extlangs.find(to_lower(lang));
	if (entry == data->extlangs.end())
		return nullptr;
	return &entry->second;
}

static const std::string get_region_code(const std::string &lang, const std::string &code)
{
	if (code[0] == 'r' && isupper(code[1]) && isupper(code[2]))
	{
		// This is an Android resource region code:
		if (lang == "es" && code == "rUS")
			return "419";
		return code.substr(1);
	}
	return code;
}

lang::tag lang::make_lang(const std::string &code)
{
	const lang::tag *alias = lookup_alias(code);
	if (alias)
		return *alias;

	lang::tag lang { "" };

	bool is_private = false;
	std::string::size_type a = 0;
	std::string::size_type b = 0;
	int n = 0;
	do
	{
		b = code.find('-', a);
		std::string item = (b == std::string::npos) ? code.substr(a) : code.substr(a, b-a);

		if (is_private)
			lang.private_use = item;
		else if (lang.lang.empty())
		{
			const lang::tag *extlang = lookup_extlang(item);
			if (extlang)
				lang = *extlang;
			else
				lang.lang = item;
		}
		else switch (item.length())
		{
		case 4:
			if (lang.script.empty())
				lang.script = item;
			else if (lang.variant.empty())
				lang.variant = item;
			break;
		case 3:
			if (lang.extlang.empty())
			{
				const lang::tag *extlang = lookup_extlang(item);
				if (extlang && extlang->lang == lang.lang)
					lang.extlang = extlang->extlang;
				else
					lang.region = get_region_code(lang.lang, item);
			}
			else
				lang.region = get_region_code(lang.lang, item);
			break;
		case 2:
			lang.region = item;
			break;
		case 1:
			if (item == "*")
			{
				if (lang.script.empty())
					lang.script = item;
			}
			else if (item == "x" || item == "X")
				is_private = true;
			else
				lang.variant = item;
			break;
		default:
			lang.variant = item;
			break;
		}

		a = b+1;
		++n;
	} while (b != std::string::npos);

	return { to_lower(lang.lang),
	         to_lower(lang.extlang),
	         capitalize(lang.script),
	         to_upper(lang.region),
	         lang.variant,
	         to_lower(lang.private_use) };
}

bool lang::operator==(const tag &a, const tag &b)
{
	if (a.variant.empty() || b.variant.empty())
	{
		if (a.region.empty() || b.region.empty())
		{
			if (a.script.empty() || b.script.empty())
				return a.lang == b.lang;
			return a.lang == b.lang && a.script == b.script;
		}
		else if (a.script == "*" || b.script == "*")
			return a.lang == b.lang && a.region == b.region;
		return a.lang == b.lang && a.script == b.script && a.region == b.region;
	}
	return a.lang == b.lang && a.script == b.script && a.region == b.region && a.variant == b.variant;
}

bool lang::operator<(const tag &a, const tag &b)
{
	if (a.lang <  b.lang) return true;
	if (a.lang != b.lang) return false;

	if (a.script < b.script) return true;

	if (a.region < b.region) return true;

	return a.variant < b.variant;
}

bool lang::issubtag(const tag &a, const tag &b)
{
	if (b.variant.empty())
	{
		if (b.region.empty())
		{
			if (b.script.empty())
				return a.lang == b.lang;
			return a.lang == b.lang && a.script == b.script;
		}
		if (b.script == "*")
			return a.lang == b.lang && a.region == b.region;
		if (b.private_use.empty())
			return a.lang == b.lang && a.script == b.script && a.region == b.region;
		return a.lang == b.lang && a.script == b.script && a.region == b.region && a.private_use == b.private_use;
	}
	return a.lang == b.lang && a.script == b.script && a.region == b.region && a.variant == b.variant;
}

std::string cainteoir::languages::language(const lang::tag &id) const
{
	if (!id.extlang.empty())
		return localize_subtag("iso_639", id.extlang);
	return localize_subtag("iso_639", id.lang);
}

std::string cainteoir::languages::script(const lang::tag &id) const
{
	return localize_subtag("iso_15924", id.script);
}

std::string cainteoir::languages::region(const lang::tag &id) const
{
	return localize_subtag("iso_3166", id.region);
}

std::string cainteoir::languages::operator()(const std::string & langid)
{
	lang::tag lang = lang::make_lang(langid);

	std::ostringstream name;
	name << language(lang);
	if (!lang.region.empty())
		name << " (" << region(lang) << ")";

	return name.str();
}
