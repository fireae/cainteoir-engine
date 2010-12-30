/* RDF formatters.
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

#include <cainteoir/metadata.hpp>

namespace rdf = cainteoir::rdf;

const rdf::ns rdf::rdf( "rdf",  "http://www.w3.org/1999/02/22-rdf-syntax-ns");
const rdf::ns rdf::rdfs("rdfs", "http://www.w3.org/2000/01/rdf-schema");
const rdf::ns rdf::xsd( "xsd",  "http://www.w3.org/2001/XMLSchema");
const rdf::ns rdf::xml( "xml",  "http://www.w3.org/XML/1998/namespace");
const rdf::ns rdf::owl( "owl",  "http://www.w3.org/2002/07/owl");

const rdf::ns rdf::dc(     "dc",      "http://purl.org/dc/elements/1.1/");
const rdf::ns rdf::dcterms("dcterms", "http://purl.org/dc/terms/");

const rdf::ns rdf::opf( "opf",  "http://www.idpf.org/2007/opf");
const rdf::ns rdf::ocf( "ocf",  "urn:oasis:names:tc:opendocument:xmlns:container");

const rdf::ns rdf::foaf("foaf", "http://xmlns.com/foaf/0.1/");
const rdf::ns rdf::tts( "tts",  "http://rhdunn.github.com/2010/12/text-to-speech");
