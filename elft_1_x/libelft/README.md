`libelft`
=========

`libelft` is a shared library containing implementations for methods defined in
[`elft.h`]. *Core* ELFT libraries must link against `libelft`.

NIST will build a clean copy of `libelft` and link it against the main
evaluation executable along with the ELFT core library. This is to ensure that
the implementations of `libelft` are equivalent for all participants, and is
additionally the reason why these methods were not implemented directly in
[`elft.h`].

Building
--------
Use the included `CMakeLists.txt` to build out of source. This will build
`libelft.so`.

```
mkdir build
cd build
cmake ..
make
```
Linking
-------
When building a core ELFT library, use these example compiler flags (from
`g++`) to properly link against this library.

> `-L/path/containing/libelft -lelft -Wl,-rpath,/path/containing/libelft`

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please [open an issue]. Other questions may be addressed
to the [NIST ELFT team].

The ELFT team sends updates about the ELFT tests to their mailing list. Enter
your e-mail address on the [mailing list site], or send a blank e-mail to
ELFT+subscribe@list.nist.gov to be automatically subscribed. Posts to the list
are mirrored on an [RSS feed].

License
-------
The items in this repository are released in the public domain. See the
[LICENSE] for details.

[`elft.h`]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include/elft.h
[NIST ELFT team]: mailto:elft@nist.gov
[open an issue]: https://github.com/usnistgov/elft/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/elft/join
[RSS feed]: https://groups.google.com/a/list.nist.gov/forum/feed/elft/msgs/rss.xml
[LICENSE]: https://github.com/usnistgov/elft/blob/master/LICENSE.md
