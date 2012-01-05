/* XML Reader API.
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

#ifndef CAINTEOIR_ENGINE_XMLREADER_HPP
#define CAINTEOIR_ENGINE_XMLREADER_HPP

#include <cainteoir/buffer.hpp>
#include <list>
#include <map>

namespace cainteoir { namespace xml
{
	struct resource
	{
		virtual const resource *clone() const = 0;

		virtual ~resource() {}
	};

	class uri : public resource
	{
	public:
		std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS = std::string(), const std::string &aRef = std::string());

		bool empty() const;

		std::string str() const;

		const resource *clone() const;
	};

	inline bool operator==(const uri &a, const uri &b)
	{
		return a.ns == b.ns && a.ref == b.ref;
	}

	inline bool operator!=(const uri &a, const uri &b)
	{
		return !(a == b);
	}

	class ns
	{
	public:
		std::string prefix; /**< @brief The default prefix for the namespace. */
		std::string href;   /**< @brief The path of the namespace. */

		ns(const std::string &aPrefix, const std::string &aHref)
			: href(aHref)
			, prefix(aPrefix)
		{
		}

		/** @brief Create a URI in the namespace.
		  *
		  * @param aRef The URI reference relative to the namespace.
		  */
		uri operator()(const std::string &aRef) const
		{
			return uri(href, aRef);
		}
	};

	inline bool operator==(const ns &a, const char *b)
	{
		return a.href == b;
	}

	inline bool operator==(const ns &a, const std::string &b)
	{
		return a.href == b;
	}

	inline bool operator==(const ns &a, const ns &b)
	{
		return a.prefix == b.prefix && a.href == b.href;
	}

	template <typename T>
	inline bool operator==(const T & a, const ns &b)
	{
		return b == a;
	}

	template <typename T>
	inline bool operator!=(const ns &a, const T &b)
	{
		return !(a == b);
	}

	template <typename T>
	inline bool operator!=(const T &a, const ns &b)
	{
		return !(b == a);
	}

	namespace xmlns
	{
		extern const ns xhtml;
		extern const ns xml;
	}

	class namespaces
	{
	public:
		namespaces();

		namespaces &add_namespace(const std::string &aPrefix, const std::string &aHref);

		namespaces &add_namespace(const cainteoir::buffer &aPrefix, const std::tr1::shared_ptr<cainteoir::buffer> &aHref)
		{
			return add_namespace(aPrefix.str(), aHref->str());
		}

		namespaces &add_namespace(const ns &aNS)
		{
			return add_namespace(aNS.prefix, aNS.href);
		}

		void push_block();
		void pop_block();

		std::string lookup(const std::string &aPrefix) const;

		std::string lookup(const cainteoir::buffer &aPrefix) const
		{
			return lookup(aPrefix.str());
		}
	private:
		struct namespace_item
		{
			long block;
			ns   item;
		};

		std::list<namespace_item> mNamespaces;
		long mBlockNumber;
	};

	class context
	{
	public:
		enum parse_flags
		{
			implicit_end_tag = 1, // implicit close tag -- <node> is the same as <node/>
		};

		struct entry
		{
			const char *name;
			uint32_t id;
			uint32_t context;
			uint32_t parameter;
			parse_flags parse_type;
		};

		void set_nodes(const std::string &ns, const std::initializer_list<const entry> &entries)
		{
			mNodes[ns] = entries;
		}

		void set_nodes(const ns &aNS, const std::initializer_list<const entry> &entries)
		{
			mNodes[aNS.href] = entries;
		}

		void set_attrs(const std::string &ns, const std::initializer_list<const entry> &entries)
		{
			mAttrs[ns] = entries;
		}

		void set_attrs(const ns &aNS, const std::initializer_list<const entry> &entries)
		{
			mAttrs[aNS.href] = entries;
		}

		const entry *lookup_node(const std::string &ns, const cainteoir::buffer &node) const
		{
			return lookup(ns, node, mNodes);
		}

		const entry *lookup_attr(const std::string &ns, const cainteoir::buffer &node) const
		{
			return lookup(ns, node, mAttrs);
		}
	private:
		const entry *lookup(const std::string &ns, const cainteoir::buffer &node, const std::map<std::string, std::initializer_list<const entry>> &entries) const;

		std::map<std::string, std::initializer_list<const entry>> mNodes;
		std::map<std::string, std::initializer_list<const entry>> mAttrs;
	};

	extern const context::entry unknown_context;

	class reader : public context
	{
	public:
		enum node_type
		{
			beginTagNode,
			endTagNode,
			processingInstructionNode,
			commentNode,
			cdataNode,
			textNode,
			error,
			doctypeNode,
			attribute,
		};

		reader(std::tr1::shared_ptr<cainteoir::buffer> aData);

		bool read();

		const cainteoir::rope &nodeValue() const { return mNodeValue; }

		const cainteoir::buffer &nodeName() const { return mNodeName; }

		const cainteoir::buffer &nodePrefix() const { return mNodePrefix; }

		std::string namespaceUri() const;

		node_type nodeType() const { return mNodeType; }

		bool isPlainText() const { return mParseAsText; }

		const context::entry *context() const { return mContext; }
	private:
		/** @name parser internals/helpers */
		//@{

		void skip_whitespace();

		bool expect_next(char c);
		bool check_next(char c);

		cainteoir::buffer identifier();

		void read_node_value(char terminator);

		void read_tag(node_type aType);

		std::tr1::shared_ptr<cainteoir::buffer> mData;
		const char * mCurrent;
		bool mParseAsText;
		bool mParseNamespaces;
		bool mImplicitEndTag;

		namespaces mNamespaces;
		cainteoir::buffer mTagNodeName;
		cainteoir::buffer mTagNodePrefix;

		//@}
		/** @name reader data */
		//@{

		cainteoir::rope mNodeValue;
		cainteoir::buffer mNodeName;
		cainteoir::buffer mNodePrefix;
		node_type mNodeType;
		const xml::context::entry *mContext;

		//@}
	};

	inline bool operator==(const reader &a, const uri &b)
	{
		return uri(a.namespaceUri(), a.nodeName().str()) == b;
	}

	inline bool operator==(const uri &a, const reader &b)
	{
		return b == a;
	}

	inline bool operator!=(const reader &a, const uri &b)
	{
		return !(a == b);
	}

	inline bool operator!=(const uri &a, const reader &b)
	{
		return !(b == a);
	}
}}

#endif
