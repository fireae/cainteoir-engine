#!/usr/bin/python

# Copyright (C) 2013-2014 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('phonemeset', sys.argv)
	test.run({'name': 'Feature-Based', 'groups': [
		{'name': 'Features', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'features', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.features', 'from': 'features', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.features', 'from': 'features', 'to': 'features'},
		]},
	]})
	test.run({'name': 'IPA-Based', 'groups': [
		{'name': 'IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.ipa', 'result': 'phonemeset/ipa/disjoint.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.ipa', 'from': 'features', 'to': 'ipa'},
			{'test': 'phonemeset/ipa/pauses.ipa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'ipa', 'to': 'features'},
			{'test': 'phonemeset/ipa/ipa-aliases.ipa', 'result': 'phonemeset/ipa/ipa-aliases.default', 'from': 'ipa', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'X-SAMPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.x-sampa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.x-sampa', 'result': 'phonemeset/ipa/disjoint.x-sampa-features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.x-sampa', 'from': 'features', 'to': 'x-sampa'},
			{'test': 'phonemeset/ipa/pauses.x-sampa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'x-sampa', 'to': 'features'},
			{'test': 'phonemeset/ipa/x-sampa-aliases.x-sampa', 'result': 'phonemeset/ipa/x-sampa-aliases.default', 'from': 'x-sampa', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'CXS', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.cxs', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.cxs', 'result': 'phonemeset/ipa/disjoint.cxs-features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.cxs', 'from': 'features', 'to': 'cxs'},
			{'test': 'phonemeset/ipa/pauses.cxs', 'result': 'phonemeset/ipa/pauses.features', 'from': 'cxs', 'to': 'features'},
		]},
		{'name': 'ASCII-IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ascii-ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.ascii-ipa', 'result': 'phonemeset/ipa/disjoint.ascii-ipa-features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa'},
			{'test': 'phonemeset/ipa/pauses.ascii-ipa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'ascii-ipa', 'to': 'features'},
			{'test': 'phonemeset/ipa/x-sampa-aliases.x-sampa', 'result': 'phonemeset/ipa/x-sampa-aliases.default', 'from': 'cxs', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
		]},
	]})
	test.run({'name': 'Arpabet-Based', 'groups': [
		{'name': 'CMU', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/cmu-phonemes.ipa', 'result': 'phonemeset/arpabet/cmu-phonemes.cmu', 'from': 'ipa', 'to': 'cmu'},
			{'test': 'phonemeset/arpabet/cmu-phonemes.cmu', 'result': 'phonemeset/arpabet/cmu-phonemes.ipa', 'from': 'cmu', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.cmu', 'from': 'ipa', 'to': 'cmu'},
			{'test': 'phonemeset/arpabet/cmu-pauses.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'cmu', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses-no-last-eol.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'cmu', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'cmu'},
			# Here, the last phoneme is a complete phoneme and the start of a diphthong.
			# Therefore, the phoneme cannot be acted on until the next phoneme is read,
			# but there is no next phoneme.
			{'test': 'phonemeset/arpabet/cmu-ambiguous-last-phoneme.ipa', 'result': 'phonemeset/arpabet/cmu-ambiguous-last-phoneme.cmu', 'from': 'ipa', 'to': 'cmu'},
		]},
		{'name': 'radio', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/radio-phonemes.ipa', 'result': 'phonemeset/arpabet/radio-phonemes.radio', 'from': 'ipa', 'to': 'radio'},
			{'test': 'phonemeset/arpabet/radio-phonemes.radio', 'result': 'phonemeset/arpabet/radio-phonemes.ipa', 'from': 'radio', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.radio', 'from': 'ipa', 'to': 'radio'},
			{'test': 'phonemeset/arpabet/cmu-pauses.radio', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'radio', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'radio'},
		]},
		{'name': 'mrpa', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/mrpa-phonemes.ipa', 'result': 'phonemeset/arpabet/mrpa-phonemes.mrpa', 'from': 'ipa', 'to': 'mrpa'},
			{'test': 'phonemeset/arpabet/mrpa-phonemes.mrpa', 'result': 'phonemeset/arpabet/mrpa-phonemes.ipa', 'from': 'mrpa', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.mrpa', 'from': 'ipa', 'to': 'mrpa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.mrpa', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'mrpa', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'mrpa'},
		]},
		{'name': 'timit', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/timit-phonemes.ipa', 'result': 'phonemeset/arpabet/timit-phonemes.timit', 'from': 'ipa', 'to': 'timit'},
			{'test': 'phonemeset/arpabet/timit-phonemes.timit', 'result': 'phonemeset/arpabet/timit-phonemes.ipa', 'from': 'timit', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.radio', 'from': 'ipa', 'to': 'timit'},
			{'test': 'phonemeset/arpabet/cmu-pauses.radio', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'timit', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'timit'},
		]},
		{'name': 'en-US', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/en-US-phonemes.ipa', 'result': 'phonemeset/arpabet/en-US-phonemes.arpabet', 'from': 'ipa', 'to': 'arpabet/en-US'},
			{'test': 'phonemeset/arpabet/en-US-phonemes.arpabet', 'result': 'phonemeset/arpabet/en-US-phonemes.ipa', 'from': 'arpabet/en-US', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.cmu', 'from': 'ipa', 'to': 'arpabet/en-US'},
			{'test': 'phonemeset/arpabet/cmu-pauses.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'arpabet/en-US', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'arpabet/en-US'},
		]},
		{'name': 'en-GB-x-rp', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/en-GB-x-rp-phonemes.ipa', 'result': 'phonemeset/arpabet/en-GB-x-rp-phonemes.arpabet', 'from': 'ipa', 'to': 'arpabet/en-GB-x-rp'},
			{'test': 'phonemeset/arpabet/en-GB-x-rp-phonemes.arpabet', 'result': 'phonemeset/arpabet/en-GB-x-rp-phonemes.ipa', 'from': 'arpabet/en-GB-x-rp', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.cmu', 'from': 'ipa', 'to': 'arpabet/en-GB-x-rp'},
			{'test': 'phonemeset/arpabet/cmu-pauses.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'arpabet/en-GB-x-rp', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-invalid-diphthong.ipa', 'result': 'phonemeset/arpabet/cmu-invalid-diphthong.cmu', 'from': 'ipa', 'to': 'arpabet/en-GB-x-rp'},
		]},
	]})
	test.run({'name': 'SAMPA-Based', 'groups': [
		{'name': 'English', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/sampa/en.ipa', 'result': 'phonemeset/sampa/en.sampa', 'from': 'ipa', 'to': 'sampa/en'},
			{'test': 'phonemeset/sampa/en.sampa', 'result': 'phonemeset/sampa/en.ipa', 'from': 'sampa/en', 'to': 'ipa'},
		]},
	]})
	test.run({'name': 'eSpeak-Based', 'groups': [
		{'name': 'English', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en'},
			{'test': 'phonemeset/espeak/en', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en', 'to': 'ipa'},
		]},
		{'name': 'English (British)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en-GB'},
			{'test': 'phonemeset/espeak/en', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en-GB', 'to': 'ipa'},
		]},
		{'name': 'English (Scottish)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en-GB-scotland.ipa', 'result': 'phonemeset/espeak/en-GB-scotland.espeak', 'from': 'ipa', 'to': 'espeak/en-GB-scotland'},
			{'test': 'phonemeset/espeak/en-GB-scotland', 'result': 'phonemeset/espeak/en-GB-scotland.ipa', 'from': 'espeak/en-GB-scotland', 'to': 'ipa'},
		]},
		{'name': 'English (Lancastrian)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en-GB-x-gbclan'},
			{'test': 'phonemeset/espeak/en', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en-GB-x-gbclan', 'to': 'ipa'},
		]},
		{'name': 'English (Received Pronunciation)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en-GB-x-rp.ipa', 'result': 'phonemeset/espeak/en-GB-x-rp.espeak', 'from': 'ipa', 'to': 'espeak/en-GB-x-rp'},
			{'test': 'phonemeset/espeak/en-GB-x-rp', 'result': 'phonemeset/espeak/en-GB-x-rp.ipa', 'from': 'espeak/en-GB-x-rp', 'to': 'ipa'},
		]},
		{'name': 'English (West Midlands)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en-GB-x-gbcwmd'},
			{'test': 'phonemeset/espeak/en', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en-GB-x-gbcwmd', 'to': 'ipa'},
		]},
		{'name': 'English (American)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en-US.ipa', 'result': 'phonemeset/espeak/en-US.espeak', 'from': 'ipa', 'to': 'espeak/en-US'},
			{'test': 'phonemeset/espeak/en-US', 'result': 'phonemeset/espeak/en-US.ipa', 'from': 'espeak/en-US', 'to': 'ipa'},
		]},
		{'name': 'English (Caribbean)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en-029.ipa', 'result': 'phonemeset/espeak/en-029.espeak', 'from': 'ipa', 'to': 'espeak/en-029'},
			{'test': 'phonemeset/espeak/en-029', 'result': 'phonemeset/espeak/en-029.ipa', 'from': 'espeak/en-029', 'to': 'ipa'},
		]},
		{'name': 'Spanish (Spain)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/es.ipa', 'result': 'phonemeset/espeak/es.espeak', 'from': 'ipa', 'to': 'espeak/es'},
			{'test': 'phonemeset/espeak/es', 'result': 'phonemeset/espeak/es.ipa', 'from': 'espeak/es', 'to': 'ipa'},
		]},
		{'name': 'Spanish (Spain)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/es.ipa', 'result': 'phonemeset/espeak/es.espeak', 'from': 'ipa', 'to': 'espeak/es'},
			{'test': 'phonemeset/espeak/es', 'result': 'phonemeset/espeak/es.ipa', 'from': 'espeak/es', 'to': 'ipa'},
		]},
		{'name': 'Spanish (Latin America)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/es.ipa', 'result': 'phonemeset/espeak/es.espeak', 'from': 'ipa', 'to': 'espeak/es-419'},
			{'test': 'phonemeset/espeak/es', 'result': 'phonemeset/espeak/es.ipa', 'from': 'espeak/es-419', 'to': 'ipa'},
		]},
		{'name': 'Language Switching', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/multi-lang.espeak', 'result': 'phonemeset/espeak/multi-lang.ipa', 'from': 'espeak/en', 'to': 'ipa'},
		]},
	]})
	test.summary()
