# Cainteoir Text-to-Speech Engine

- [Building](#building)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Cainteoir Text-to-Speech engine is a library that provides text-to-speech
functionality for reading and recording different document formats.

## Building

The Cainteoir Engine supports the standard GNU autotools build system. The
source code does not contain the generated `configure` files, so to build
it you need to run:

	./autogen.sh
	./configure --prefix=/usr
	make

The tests can be run by using:

	make check

The program can be installed using:

	sudo make install

The documentation can be built (after installing `pandoc`) using:

	make doc

Source tarballs can be generated by running:

	make dist

## Bugs

Report bugs to the [cainteoir-engine issues](https://github.com/rhdunn/cainteoir-engine/issues)
page on GitHub.

## License Information

The Cainteoir Text-to-Speech Engine is released under the GPL version 3 or later license.

Cainteoir is a registered trademark of Reece Dunn.

W3C is a trademark (registered in numerous countries) of the World Wide Web Consortium; marks of W3C are registered and held by its host institutions MIT, ERCIM, and Keio.

All trademarks are property of their respective owners.
