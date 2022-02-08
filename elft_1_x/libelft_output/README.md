`libelft_output`
================

`libelft_output` is a shared library containing output stream operator overload
and string conversion methods for types defined in [`elft.h`]. It can be used to
help debug your ELFT implementation.

**Do _not_ link against `libelft_output` when submitting to ELFT.** NIST has a
different implementation of the methods defined here, which _may_ expose linkage
problems. NIST also requires that libraries not output to the console, so
removing this library and stream header include directives will help ensure you
don't accidentally write output.

Building
--------
Use the included `CMakeLists.txt` to build out of source. This will build
`libelft_output.so` and its dependency, [`libelft`].

```
mkdir build
cd build
cmake ..
make
```

Linking
-------
When building another program, use these example compiler flags (from `g++`)
to properly link against this library.

> `-L/path/containing/lib -lelft_output -Wl,-rpath,/path/containing/lib`

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
[`elft.h`]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include/elft.h
[NIST ELFT team]: mailto:elft@nist.gov
[open an issue]: https://github.com/usnistgov/elft/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/elft/join
[LICENSE]: https://github.com/usnistgov/elft/blob/master/LICENSE.md
