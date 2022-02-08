ELFT API 1.x
============

The contents of this directory contains files needed to participate in the
**1.x** revision of [ELFT].

 * [include]
   - The header file that defines the [ELFT 1.x API]. [Browsable] and
     [printable] documentation for the API generated from `include/elft.h` is
     available.
 * [libelft]
   - Code to build a supporting library that implements methods declared in
     `include/elft.h`.
 * [nullimpl]
   - A stub library compliant with the [ELFT 1.x API] that can be used as a
     starting point for development.

It also contains some optional but perhaps useful components:

 * [libelft_output]
   - A library containing methods to convert types defined in the [ELFT 1.x API]
     to `std::string` for debugging.
 * [randimpl]
   - An implementation of the [ELFT 1.x API] that writes random data, in an
     effort to provide code examples of how some features of the API might be
     used.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please [open an issue]. Other questions may be addressed
to the [NIST ELFT team].

The ELFT team sends updates about the ELFT tests to their mailing list. Enter
your e-mail address on the [mailing list site], or send a blank e-mail to
ELFT+subscribe@list.nist.gov to be automatically subscribed.

License
-------
The items in this repository are released in the public domain. See the
[LICENSE] for details.

[nullimpl]: https://github.com/usnistgov/elft/blob/master/elft_1_x/nullimpl
[libelft]: https://github.com/usnistgov/elft/blob/master/elft_1_x/libelft
[include]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include
[ELFT 1.x API]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include/elft.h
[open an issue]: https://github.com/usnistgov/elft/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/elft/join
[LICENSE]: https://github.com/usnistgov/elft/blob/master/LICENSE.md
[NIST ELFT team]: mailto:elft@nist.gov
[ELFT]: https://www.nist.gov/image-group/elft
[Browsable]: https://pages.nist.gov/elft/elft_1_x/doc/api
[printable]: https://pages.nist.gov/elft/elft_1_x/doc/api.pdf
[randimpl]: https://github.com/usnistgov/elft/blob/master/elft_1_x/randimpl
[libelft_output]: https://github.com/usnistgov/elft/blob/master/elft_1_x/libelft_output
