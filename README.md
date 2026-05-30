Feathercoin Core
================

What is Feathercoin?
--------------------

Feathercoin is a fork of the Bitcoin Core project. It contains its own
set up of certain parameters, such as block times and extra features such
as enhanced mining difficulty calculations, protection against 51% attacks,
and its own hashing algorithm.

Feathercoin is an open source project and additional changes and fixes to
the source code are managed by the community.

Telegram: https://t.me/FeathercoinOfficial  
Twitter: https://twitter.com/Feathercoin  
Forum: http://forum.feathercoin.com/

What is Feathercoin?
--------------------

Feathercoin is an experimental digital currency that enables instant payments
to anyone, anywhere in the world. Feathercoin uses peer-to-peer technology to
operate with no central authority: managing transactions and issuing money are
carried out collectively by the network. Feathercoin Core is the name of open
source software which enables the use of this currency.

For more information, as well as an immediately usable, binary version of the
Feathercoin Core software, see the
[Feathercoin releases page](https://github.com/FeatherCoin/Feathercoin/releases).

Apple Silicon macOS users should note that the Feathercoin Core 0.21.0 macOS
release package is x86_64-only and requires Rosetta. To run without Rosetta,
build current `master` natively on Apple Silicon; see
[doc/build-osx.md](doc/build-osx.md).

License
-------

Feathercoin Core is released under the terms of the MIT license. See
[COPYING](COPYING) for more information or see
https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but it is not guaranteed to
be completely stable. [Tags](https://github.com/FeatherCoin/Feathercoin/tags)
are created regularly to indicate new official, stable release versions of
Feathercoin Core.

The contribution workflow is described in
[CONTRIBUTING.md](CONTRIBUTING.md) and useful hints for developers can be found
in [doc/developer-notes.md](doc/developer-notes.md).

Testing
-------

Testing and code review is the bottleneck for development. Please be patient
and help out by testing other people's pull requests, and remember this is a
security-critical project where any mistake might cost people money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md)
for new code, and to submit new unit tests for old code. Unit tests can be
compiled and run (assuming they were not disabled in configure) with:
`make check`. Further details on running and extending unit tests can be found
in [/src/test/README.md](/src/test/README.md).

There are also [functional and integration tests](/test), written in Python,
that are run automatically on the build server. These tests can be run (if the
[test dependencies](/test) are installed) with:
`test/functional/test_runner.py`

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
