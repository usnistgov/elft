ELFT Random Implementation
==========================

This directory contains example code that implements a naive file system
structure for a reference database, to be used internally for testing
[ELFT 1.x API] test driver code. It does **not** perform any sort of *real*
feature extraction or searching.

Building
--------
Use the included `CMakeLists.txt` to build out of source. This will build
`libelft_randimpl_0001.so` and its dependency, [`libelft`].

```
mkdir build
cd build
cmake ..
make
```

The name and version number for the library to be built are extracted from
within the C++ source.

Configuration
-------------
This implementation makes use of a configuration file named `seed` that contains
a single line with an unsigned 32-bit integer seed for the random number
generator. This enables predictable randomized outputs and failures.

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

[`libelft`]: https://github.com/usnistgov/elft/blob/master/elft_1_x/libelft
[NIST ELFT team]: mailto:elft@nist.gov
[open an issue]: https://github.com/usnistgov/elft/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/elft/join
[LICENSE]: https://github.com/usnistgov/elft/blob/master/LICENSE.md
[ELFT 1.x API]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include/elft.h
