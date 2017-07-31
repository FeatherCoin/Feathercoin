
Feathercoin Core
================

Status - Feathercoin 0.11.x :  Beta Test
--------------------------------------------------------------


What is Feathercoin?
--------------------

Feathercoin is a Fork of the Bitcoin core project. It contains it's own set up of certain parameters, such as block times, extra features such as 
enhanced mining difficulty calculations and it's own encryption algorithm.

Like Bitcoin, Feathercoin is an open source project and additional changes and fixes to those in the source code are managed by the forum community as is, depending on
the change or scale of release.

**Forum :**
http://forum.feathercoin.com/

**Development mailing list / Forum Thread :**
http://forum.feathercoin.com/category/71/technical-development

Feathercoin is an experimental digital currency that enables instant payments to anyone, anywhere in the world. Feathercoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried out collectively by the network. Feathercoin Core is the name of open source software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of the Feathercoin Core software, see these [Downloads](https://github.com/FeatherCoin/Feathercoin/releases).

**Specifications :**
*  1 minute block targets</li>
*  40 coins per block</li>
*  NeoScrypt hashing algorithm</li>
*  Retarget every block</li>
*  Retarget with average of 15, 120 and 480 block averages and .25 damping</li>
*  Subsidy halves every ~4 years</li>
*  ~336 million total coins</li>



License
-------

Feathercoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `0.11.2` branch is regularly built and tested, but is not guaranteed to be completely stable. [Tags](https://github.com/Feathercoin/Feathercoin/tags) are created regularly to indicate new official, stable release versions of Feathercoin Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).


Testing
-------

Testing and code review is the bottleneck for development; we get more pull requests than we can review and test on short notice. Please be patient and help out by testing other people's pull requests, and remember this is a security-critical project where any mistake might cost people lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](/doc/unit-tests.md) for new code, and to submit new unit tests for old code. Unit tests can be compiled and run (assuming they weren't disabled in configure) with: `make check`

There are also [regression and integration tests](/qa) of the RPC interface, written in Python, that are run automatically on the build server. These tests can be run (if the [test dependencies](/qa) are installed) with: `qa/pull-tester/rpc-tests.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the code. This is especially important for large or high-risk changes. It is useful to add a test plan to the pull request description if testing the changes is not straightforward.

