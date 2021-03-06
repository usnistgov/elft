ELFT Validation
===============

We require exercising ELFT [API] implementations with the ELFT validation
package. Validation is mutually-beneficial to NIST and ELFT participants. The
hope is that successful execution of validation ensures your algorithm:

 * runs as expected at NIST;
 * does not crash;
 * syntactically and semantically implements the [API] correctly.

This helps cut the runtime of the evaluation and gives a higher level of
confidence that the results presented by NIST are a true measure of the
submitted software.

Contents
--------
 * Interaction Required:
   - **[config/]:** Directory in which all configuration files required by your
     libraries reside. This directory will be read-only at runtime. Use of
     configurations is optional.
   - **[lib64/]:** Directory in which all required libraries reside. There must
     be at least library, the **core** library, and that library **must** follow
     the ELFT naming convention.
   - **[../libelft/]:** Code for the shared library implementing methods
     declared in [../include/elft.h].
   - **[../include/elft.h]:** The ELFT [API].
   - **[validate]:** Script that automates running the validation and performing
     checks on the output.
 * Supporting Files
   - **[README.md]:** This file.
   - **[src/]:** Source code to build the validation executable.
   - **[VERSION]** Version number of the validation package.

Requirements
------------

 * Fingerprint Imagery
   - Because organizations must agree to NIST Special Database terms and
     conditions, the required fingerprint imagery is not included in this GitHub
     repository. Request and download the data from our [requests website].
 * CentOS 8.2.2004
   - Even if this is not the latest version of CentOS, it will be the version
     used to run the evaluation. Direct downloads are available from the [CentOS
     Vault] ([🇺🇸 USA], [🇪🇺 Europe]). We will be using the base CentOS
     Linux distribution (frozen) **not CentOS Stream**.
   - We **highly suggest** installing this version of CentOS Linux directly from
     the ISO without a network connection so that versions of all packages
     are consistent with our operating environment.
   - The [validate] script  requires these base CentOS packages:
      - `binutils`, `centos-release`, `coreutils`, `cmake`, `curl`, `dnf`,
        `file`, `gawk`, `gcc-c++`, `grep`, `iputils`, `make`, `sed`, `tar`,
        `which`, `xz`

It is **highly suggested** that you make sure your submission will build and run
as expected on environments as close as possible to the NIST evaluation
machines, in order to avoid validation delays. Timing statistics are reported
when run on a **Intel Xeon Gold 6140** CPU, but all Intel CPU types shall be
supported. Use of unavailable intrinsics shall degrade gracefully.

How to Run
----------
 1. Put your compiled core library and any other required libraries in [lib64/].
 2. Put any required configuration files in [config/].
 3. Put ELFT fingerprint imagery received from NIST in this directory (i.e.,
    the directory containing this file, [README.md]).
 4. Execute [validate] (`./validate`).
 5. **If successful**, sign *and* encrypt the resulting output archive in a
    single step, and e-mail it, along with the encrypting identity's public key,
    to elft@nist.gov. For an example of how to use GnuPG to encrypt, run
    `validate encrypt`. If unsuccessful, correct any errors described and try
    again.

<details>
  <summary><em>Expand to view the output from an example run.</em></summary>

```
$ cp /path/to/libelft_nullimpl_0001.so lib64/
$ cp /path/to/config.txt config/
$ cp /path/to/elft_validation_images-*.tar.xz .
$ ./validate
================================================================================
|     ELFT Validation | Version 202103121022 | 12 Mar 2021 | 10:53:28 EST      |
================================================================================
Checking for required packages... [OKAY]
Checking for previous validation attempts... [OKAY]
Checking validation version... (no Internet connection) [SKIP]
Checking OS and version... (CentOS 8.2.2004) [OKAY]
Checking for validation images... [DEFER]
 -> Expanding "elft_validation_images-202103120958.tar.xz"... [OKAY]
Checking for validation images... [OKAY]
Looking for core library... (libelft_nullimpl_0001.so) [OKAY]
Checking for known environment variables... [OKAY]
Building... [OKAY]
Checking API version... [OKAY]
Testing ExtractionInterface (probe)... [OKAY]
Checking probe extraction logs... [WARN]

================================================================================
| There are some (108) zero-byte probe templates. Please review:               |
| output/driver/extractionCreate-0.log                                         |
================================================================================
Still checking probe extraction logs... [OKAY]
Testing ExtractionInterface (reference)... [OKAY]
Checking reference extraction logs... [WARN]

================================================================================
| There are some (55) zero-byte reference templates. Please review:            |
| output/driver/extractionCreate-1.log                                         |
================================================================================
Still checking reference extraction logs... [OKAY]
Testing reference database creation... [OKAY]
Testing reference database modification... [OKAY]
Testing SearchInterface... [OKAY]
Checking search logs (candidates)... [WARN]

================================================================================
| There are some (432) searches that returned successfully, but did not        |
| produce any candidates. Please review:                                       |
| /mnt/isiA01/users/gfiumara/git/elft_public/elft_1_x/validation/output/driver |
| /searchCandidates.log                                                        |
================================================================================
Still checking search logs... [OKAY]
Creating validation submission... (elft_validation_nullimpl_0001.tar.xz) [OKAY]

================================================================================
| Please review the marketing and CBEFF information compiled into your         |
| library to ensure correctness:                                               |
|                                                                              |
| Exemplar Feature Extraction Algorithm Marketing Identifier =                 |
| NullImplementation Exemplar Extractor 1.0                                    |
| Exemplar Feature Extraction Algorithm CBEFF Owner = 0x000F                   |
| Exemplar Feature Extraction Algorithm CBEFF Identifier = 0xF1A7              |
| Latent Feature Extraction Algorithm Marketing Identifier =                   |
| NullImplementation Latent Extractor 1.0                                      |
| Latent Feature Extraction Algorithm CBEFF Owner = 0x000F                     |
| Latent Feature Extraction Algorithm CBEFF Identifier = 0x01AC                |
| Search Algorithm Marketing Identifier = NullImplementation Matcher 1.0       |
| Search Algorithm CBEFF Owner = 0x000F                                        |
| Search Algorithm CBEFF Identifier = 0x0101                                   |
================================================================================

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ This script could not check online to ensure there are no updates            +
+ available. NIST requires that ELFT submissions always use the latest         +
+ version. Retrieve the latest version number by visiting the URL below and    +
+ be sure it matches this version: 202103121022.                               +
+                                                                              +
+ https://github.com/usnistgov/elft/tree/master/elft_1_x/validation/VERSION    +
+                                                                              +
+ If these numbers don't match, visit our website to retrieve the latest       +
+ version.                                                                     +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

================================================================================
| You have successfully completed your part of ELFT validation. Please sign    |
| and encrypt the file listed below (run './validate encrypt' for an example). |
|                                                                              |
|                     elft_validation_nullimpl_0001.tar.xz                     |
|                                                                              |
| elft_validation_nullimpl_0001.tar.xz.asc will likely too big for NIST's      |
| email server, so please contact elft@nist.gov for an upload link for both    |
| elft_validation_nullimpl_0001.tar.xz.asc and your public key.                |
================================================================================
Completed: 12 Mar 2021 | 10:55:20 EST (Runtime: 112s)
```
</details>

Submission Contents
-------------------

 * **canary.log:** MD5 checksums of randomly-generated images we provide as part
   of the validation fingerprint imagery. This helps us make sure that you are
   using the most recent version of ELFT validation fingerprint imagery.
 * **compile.log:** Output from compiling the validation executable and other
   information like MD5 checksums, versions, and library dependencies that may
   help us debug your submission if an error occurs.
 * **config/:** A copy of [config/].
 * **driver/:** Output from the validation executable:
   * **correspondence.log:** Corresponding features from candidates generated,
     as logged in `driver/searchCandidates.log`.
   * **createReferenceDatabase.log:** Output from creating the reference
     database.
   * **extractionCreate-0.log:** Output from generating probe templates.
   * **extractionCreate-1.log:** Output from generating reference templates.
   * **extractionData-0.log:** Features extracted from probe templates.
   * **extractionData-1.log:** Features extracted from reference templates.
   * **modifyReferenceDatabase.log:** Output from modifying the reference
     database.
   * **reference_database/:** Reference database created, as logged in
    `driver/createReferenceDatabase`.
   * **searchCandidates.log:**
   * **templates/:** Templates created, as logged in
     `driver/extraction{Create,Data}-0.log`
 * **id_extract.log:** Information about the linked core library's
   `ExtractionInterface`. This is information derived from values compiled into
   the core library.
 * **id_search.log:** Information about the linked core library's
   `SearchInterface`. This is information derived from values compiled into
   the core library.
 * **lib64/:** A copy of [lib64/].
 * **run-createReferenceDatabase.log:** The command used to launch the
   validation executable when generating `driver/createReferenceDatabase.log`.
 * **run-extract-probe.log:** The command used to launch the validation
   executable when generating `driver/extraction{Create,Data}-0.log`.
 * **run-extract-reference.log:** The command used to launch the validation
   executable when generating `driver/extraction{Create,Data}-1.log`.
 * **run-modifyReferenceDatabase.log:** The command used to launch the
   validation executable when generating `driver/modifyReferenceDatabase.log`.
 * **run-search.log:** The command used to launch the
   validation executable when generating `driver/searchCandidates.log` and
   `driver/correspondence.log`.

### A Note about Paths
This following  files may contain absolute paths to files on your system. If
this information is sensitive, you may redact the paths, leaving the structure
of the file content the same.

 * canary.log
 * compile.log
 * run-createReferenceDatabase.log
 * run-extract-probe.log
 * run-extract-reference.log
 * run-modifyReferenceDatabase.log
 * run-search.log

Checks Performed
----------------

 * Implementation can handle being `fork`ed.
 * Validation package and imagery is at most recent revision level.
 * Appropriate operating system version installed.
 * Libraries and configurations can be placed randomly on disk.
 * Appropriately-named ELFT core software library is present.
 * Software library links properly against the validation driver.
 * Crashes do not occur when handling various types of imagery, including
   - atypical resolutions;
   - atypical data sources (e.g., features-only, latent probe)
   - unknown metadata;
   - non-contact imagery;
   - blank or gradient patterns.

While the validation package tries to eliminate errors from the ELFT
submission, it is not perfect, and there are still several ways in which the
package might approve you for submission that NIST may later reject.

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

[API]: https://pages.nist.gov/elft/elft_1_x/doc/api/
[CentOS Vault]: https://vault.centos.org/
[🇺🇸 USA]: https://mirrors.oit.uci.edu/centos/8.2.2004/isos/x86_64/CentOS-8.2.2004-x86_64-dvd1.iso
[🇪🇺 Europe]: http://mirror.nsc.liu.se/centos-store/8.2.2004/isos/x86_64/CentOS-8.2.2004-x86_64-dvd1.iso
[lib64/]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/lib64
[../libelft/]: https://github.com/usnistgov/elft/blob/master/elft_1_x/libelft
[../include/elft.h]: https://github.com/usnistgov/elft/blob/master/elft_1_x/include/elft.h
[bin/]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/bin
[config/]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/config
[README.md]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/README.md
[src/]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/src
[VERSION]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/VERSION
[validate]: https://github.com/usnistgov/elft/blob/master/elft_1_x/validation/validate
[NIST ELFT team]: mailto:elft@nist.gov
[open an issue]: https://github.com/usnistgov/elft/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/elft/join
[RSS feed]: https://groups.google.com/a/list.nist.gov/forum/feed/elft/msgs/rss.xml
[LICENSE]: https://github.com/usnistgov/elft/blob/master/LICENSE.md
[test plan]: https://pages.nist.gov/elft/doc/elft_1_x/testplan.pdf
[requests website]: https://nigos.nist.gov/datasets/elft_validation/request
