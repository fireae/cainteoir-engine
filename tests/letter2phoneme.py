#!/usr/bin/python

# Copyright (C) 2014 Reece H. Dunn
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
	test = harness.TestSuite('letter2phoneme', sys.argv)
	test.run({'name': 'Letter To Phoneme', 'groups': [
		{'name': 'Context', 'type': 'rewrite', 'args': ['--phonemes'], 'tests': [
			{'test': 'letter2phoneme/context.txt', 'result': 'letter2phoneme/context.ps', 'ruleset': 'letter2phoneme/context.ldb'},
			{'test': 'letter2phoneme/right.txt', 'result': 'letter2phoneme/right.ps', 'ruleset': 'letter2phoneme/context.ldb'},
			{'test': 'letter2phoneme/left.txt', 'result': 'letter2phoneme/left.ps', 'ruleset': 'letter2phoneme/context.ldb'},
		]},
		{'name': 'Rewrite', 'type': 'rewrite', 'args': ['--phonemes'], 'tests': [
			{'test': 'letter2phoneme/cantonese.txt', 'result': 'letter2phoneme/cantonese.ps', 'ruleset': 'letter2phoneme/cantonese.ldb'},
			{'test': 'letter2phoneme/mandarin.txt', 'result': 'letter2phoneme/mandarin.ps', 'ruleset': 'letter2phoneme/mandarin.ldb'},
			{'test': 'letter2phoneme/japanese.txt', 'result': 'letter2phoneme/japanese.ps', 'ruleset': 'letter2phoneme/japanese.ldb'},
			{'test': 'letter2phoneme/morpheme.txt', 'result': 'letter2phoneme/morpheme.ps', 'ruleset': 'letter2phoneme/context.ldb'},
		]},
	]})
	test.summary()
