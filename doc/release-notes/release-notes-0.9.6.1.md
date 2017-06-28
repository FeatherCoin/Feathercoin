Feathercoin Core version 0.9.0 is now available from:

  https://feathercoin.org/bin/0.9.0/

This is a new minor version release, bringing bug fixes.

Please report bugs using the issue tracker at github:

  https://github.com/FeatherCoin/Feathercoin/issues

How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), uninstall all
earlier versions of Feathercoin, then run the installer (on Windows/Linux/Mac) or just copy
over /Applications/Feathercoin-Qt (on Mac) or feathercoind/feathercoin-qt (on Linux).

If you are upgrading from version 0.8.x or earlier, the first time you run
0.9.6.1 your blockchain files will be re-indexed, which will take anywhere from 
30 minutes to several hours, depending on the speed of your machine.

On Windows, do not forget to uninstall all earlier versions of the Feathercoin
client first, especially if you are switching to the 64-bit version.

Windows
-------
0.9.6.1 Windows binaries are not code-signed; use PGP
and the SHA256SUMS.asc file to make sure your binaries are correct.


OSX 10.5 / 32-bit no longer supported
-------------------------------------

0.9.x drops support for older Macs. The minimum requirements are now:
* A 64-bit-capable CPU (see http://support.apple.com/kb/ht3696);
* Mac OS 10.6 or later (see https://support.apple.com/kb/ht1633).

Downgrading warnings
--------------------

The 'chainstate' for this release is not always compatible with previous
releases, so if you run 0.9 and then decide to switch back to a
0.8.x release you might get a blockchain validation error when starting the
old release (due to 'pruned outputs' being omitted from the index of
unspent transaction outputs).

Running the old release with the -reindex option will rebuild the chainstate
data structures and correct the problem.

Also, the first time you run a 0.8.x release on a 0.9 wallet it will rescan
the blockchain for missing spent coins, which will take a long time (tens
of minutes on a typical machine).




Autotools build system
-----------------------

For 0.9.0 we switched to an autotools-based build system instead of individual
(q)makefiles.

Using the standard "./autogen.sh; ./configure; make" to build Feathercoin-Qt and
feathercoind makes it easier for experienced open source developers to contribute 
to the project.

Be sure to check doc/build-*.md for your platform before building from source.

Feathercoin-cli
-------------

Another change in the 0.9 release is moving away from the feathercoind executable
functioning both as a server and as a RPC client. The RPC client functionality
("tell the running feathercoin daemon to do THIS") was split into a separate
executable, 'feathercoin-cli'. The RPC client code will eventually be removed from
feathercoind, but will be kept for backwards compatibility for a release or two.

Bux Fixes:
----------

- When a 0.11.x node connects, the connection will be with 'invalid pchMessageStart in 0.9.6
and earlier versions. (Github Issue #171)
This has been fixed by implementing pchMessageStartNew constant to allow future 0.11.x nodes to connect.

- The configuration parameter 'enfoce checkpoint was not handled correctly in 0.9.6, when set. 
(Github issues #169,#170)

- A formating error on the Multisig GUI page was fixed (Github issue #160)

- Log messages get id numbers (Github issue #157)

- Test section was updated ( Github issues #120 - #127)

