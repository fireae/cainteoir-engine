# Cainteoir Text-to-Speech Engine

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Documentation](#documentation)
- [Comparison With eSpeak](#comparison-with-espeak)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

The Cainteoir Text-to-Speech engine is a library that provides text-to-speech
functionality for reading and recording different document formats.

## Build Dependencies

In order to build cainteoir-engine, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`,
    `autopoint` and `pkg-config`);
2.  a functional c++ compiler;
3.  the Python YAML parser library;
4.  the zlib development libraries;
5.  the shared mime info package.

Optionally, you need:

1.  the pulseaudio development library to enable pulseaudio output;
2.  the alsa development libraries to enable alsa audio output;
3.  the vorbis encoder development libraries for ogg/vorbis support;
4.  the espeak development libraries for espeak support;
5.  the pico development libraries for svox pico support;
6.  the poppler development libraries for pdf support.

If you want ePub 3.0 Media Overlay support, you need FFmpeg or libav v9 or later,
with the following libraries installed:

1.  libavutil;
2.  libavcodec;
3.  libavformat;
4.  libavresample — for converting differently sampled audio files to the one used by the TTS voice.

To build the documentation, you need:

1.  the pandoc program to build the general documentation;
2.  the doxygen program to build the api documentation;
3.  the dot program to generate graphs in the api documentation.

### Debian

Core Dependencies:

| Dependency       | Install                                                                    |
|------------------|----------------------------------------------------------------------------|
| autotools        | `sudo apt-get install make autoconf automake libtool autopoint pkg-config` |
| c++ compiler     | `sudo apt-get install gcc g++`                                             |
| python yaml      | `sudo apt-get install python-yaml`                                         |
| zlib             | `sudo apt-get install zlib1g-dev`                                          |
| shared mime info | `sudo apt-get install shared-mime-info`                                    |

Optional Libraries:

| Dependency     | Install                                    |
|----------------|--------------------------------------------|
| pulseaudio     | `sudo apt-get install libpulse-dev`        |
| alsa           | `sudo apt-get install libasound2-dev`      |
| vorbis encoder | `sudo apt-get install libvorbis-dev`       |
| espeak         | `sudo apt-get install libespeak-dev`       |
| pico           | `sudo apt-get install libttspico-dev`      |
| poppler        | `sudo apt-get install libpoppler-glib-dev` |
| ffmpeg/libav   | `sudo apt-get install libavformat-dev libavcodec-dev` |
| libavresample  | `sudo apt-get install libavresample-dev`   |

Documentation Dependencies:

| Dependency | Install                              |
|------------|--------------------------------------|
| kramdown   | `sudo apt-get install ruby-kramdown` |
| doxygen    | `sudo apt-get install doxygen`       |
| graphviz   | `sudo apt-get install graphviz`      |

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

Source tarballs can be generated by running:

	make dist

## Documentation

The documentation can be built by running:

	make doc

Alternatively, just the API documentation can be built by running:

	make api

**NOTE:** You need a recent version of doxygen (such as 1.8.5) that supports
C++11 constructs, specifically scoped enumerations. The documentation is
affected by the following doxygen bugs:

| Bug No. | Version | Description |
|---------|---------|-------------|
| [722363](https://bugzilla.gnome.org/show_bug.cgi?id=722363) | any   | C++11 enum cannot doc two enums with the same value out-of-line |
| [722457](https://bugzilla.gnome.org/show_bug.cgi?id=722457) | 1.8.6 | regression referencing namespaced type |
| [722667](https://bugzilla.gnome.org/show_bug.cgi?id=722667) | any   | Documenting matching enumeration types out of line does not work in different namespaces |

## Comparison With eSpeak

The Cainteoir Text-to-Speech engine has support for using eSpeak to synthesize
text. It is architectured differently to eSpeak such that:

1.  The *document processing* phase is separate from the *text processing* phase.

    This means that Cainteoir Text-to-Speech can support a wider variety of text
    formats such as ePub, PDF and RTF, as well as providing support for ePub 3
    Media Overlays. The HTML and SSML processing in eSpeak is tied to the text
    processing module which makes it difficult to test and maintain.

    This allows Cainteoir TTS to correctly detect and process a HTML page that
    just contains an email document complete with MIME tags such that the email
    is processed instead of treating it as plain text.

2.  Numbers are converted to words instead of phonemes.

    This allows the numeric and word forms to be pronounced consistently and can
    more easily follow accent variations. In eSpeak they can be inconsistent, for
    example `16` and `sixteen` have a different stress placement.

    Currently, the number handling in Cainteoir TTS is not as powerful as the one
    in eSpeak as it cannot handle masculine/feminine/neuter forms from German and
    other languages and other language variations such as those found in Czech and
    Irish Gaelic.

3.  Support for large numbers.

    Numbers upto 10<sup>99</sup> can be supported in the current engine and it is
    easy to support even higher numbers. Whereas eSpeak reads `1000000000000` as
    `thousand billion` in both US and UK English instead of `trillion` for US
    English. For higher numbers, eSpeak speaks each digit individually.

Future work and evolution of Cainteoir Text-to-Speech:

1.  Separation of language from voice.

    In eSpeak, language and voice are the same thing. Thus, it is difficult to
    use a different voice for a given language (eSpeak has a hard coded map of
    supported MBROLA voices). It is also difficult to get a voice to support
    different languages (some of the MBROLA voices eSpeak supports have English
    variants, but there are no mappings to other languages).

    For Cainteoir, a *voice* is just a phoneme synthesizer, a *language* is a
    mapping from words to phonemes and an *accent* is a mapping from phonemes
    to phonemes. Thus, any of these can be mixed and matched as long as they
    have compatible phoneme sets.

2.  Support for different dictionaries and pronunciation rules.

    The dictionary and rule set architecture is such that it is easy to support
    different formats for these, so Cainteoir should support as many as
    possible. It is possible to mix and match dictionaries and pronunciation
    rules.

3.  User and document dictionaries.

    Users will want to provide customized pronunciations for words that are
    mispronounced or are not in the user's accent. Likewise, a document such
    as ePub may reference a pronunciation dictionary containing pronunciations
    of words a Text-to-Speech engine is likely to get wrong (like Latin, foreign
    or made up names).

    In eSpeak, a user must build their pronunciation changes into the main eSpeak
    dictionary file. With Cainteoir, it should be possible to add user-level and
    document-level pronunciations.

4.  Precise phonetic pronunciations.

    The sounds supported by a voice should use precise articulation based on
    the phonetic features of that phoneme, not its meaning for a speaker in
    a given language. That is, each sound should be as narrow and specific
    as possible.

    Disambiguation of allophones (e.g. aspirated plosive at the start of a
    word) is done after the conversion of text to phonemes, but before
    synthesis. Thus, allophones are a feature of the language/accent, not the
    voice.

    As such, it should be possible to use the phoneme transcription schemes
    to parse phonemes transcribed in IPA, ASCII-IPA or other scheme and feed
    that directly to the voice to be synthesized without the synthesizer
    modifying the sounds as happens with eSpeak.

5.  Generate an exception dictionary from a reference dictionary.

    In eSpeak the generation of the exception dictionary is done by hand. This
    results in some words that can be pronounced from the letter-to-phoneme
    rules being present in the dictionary. Also, changes in the
    letter-to-phoneme rules can result in some words regressing pronunciation.

    To resolve those issues, a reference dictionary should be created with a
    pronunciation of words that is known to be valid. From this, any word that
    is not pronouncable via the rule set should be added to the exception
    dictionary. This ensures that the exception dictionary is as small as
    possible and that the words in the reference dictionary are pronounced
    correctly.

    It also makes it easier to test, analyse and experiment with the rule sets.
    For example, statistics can be run to see how many times a given rule was
    matched and how many times it did not match.

6.  Support selecting which language to use to synthesize language scripts.

    The Cainteoir engine detects which script (Latin, Greek, Cyrillic, etc.)
    each section of text is written in. It should be possible to configure
    what language each script should be read in to provide a more fluent
    experience.

    The eSpeak engine has limited support for this where e.g. the Bulgarian
    language will use Bulgarian for Cyrillic and English for Latin text.
    For other languages, it speaks the language name and then the word
    `letter` which is not useful when e.g. a user is trying to learn
    Japanese.

7.  Support different voice synthesizers.

    In eSpeak, support for MBROLA phonemes is mixed in with its support for
    spectral, klatt and wave phonemes. This is done at the wrong level and
    makes it difficult to maintain.

    There should be a voice/phoneme synthesizer interface that takes the
    phonemes and associated prosodic information and generates audio from
    that. Supported synthesizers could include:

    1.  MBROLA — use the MBROLA diphone synthesizer
    2.  eSpeak — use the eSpeak spectral, klatt and wave synthesizer
    3.  Klatt  — use the Klatt synthesizer with Holmes' vocal track parameters
        (as is done by [rsynth](https://github.com/rhdunn/rsynth))
    4.  diphone concatenation synthesizer — combining recorded samples of
        phonemes from a speaker reading some text
    5.  vocal tract synthesizer — modelling the acoustic behaviour of the
        vocal tract including the vocal chords, mouth, tongue and nose

    When loading the voice, the voice data is passed to the voice synthesizer.
    This tells the voice synthesizer how to handle the different phonemes and
    generate the audio for those. For example, an MBROLA voice will provide
    the MBROLA voice name (e.g. `de1`) and a mapping of phonemes to the voice's
    phoneme set including any prosody transformations (e.g. for voices that
    do not have different phonemes for the long and short variants of a given
    vowel).

    The voice/phoneme synthesizer object is then passed to the engine so the
    engine can feed phonemes to the synthesizer after the *text to phoneme* and
    *prosodic analysis* phases.

## Bugs

Report bugs to the [cainteoir-engine issues](https://github.com/rhdunn/cainteoir-engine/issues)
page on GitHub.

## License Information

The Cainteoir Text-to-Speech Engine is released under the GPL version 3 or later license.

Cainteoir is a registered trademark of Reece Dunn.

W3C is a trademark (registered in numerous countries) of the World Wide Web Consortium; marks of W3C are registered and held by its host institutions MIT, ERCIM, and Keio.

All trademarks are property of their respective owners.
