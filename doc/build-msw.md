Copyright (c) 2009-2013 Bitcoin Developers
Copyright (c) 2013 Feathercoin Developers
Distributed under the MIT/X11 software license, see the accompanying
file COPYING or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](http://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.


See readme-qt.rst for instructions on building Feathercoin-Qt, the
graphical user interface.

WINDOWS BUILD NOTES
===================

Compilers Supported
-------------------
TODO: What works?
Note: releases are cross-compiled using mingw running on Linux.


Dependencies
------------
Libraries you need to download separately and build:

                default path               download
OpenSSL         \openssl-1.0.1e        http://www.openssl.org/source/
Berkeley DB     \db-4.8.30.NC          http://www.oracle.com/technology/software/products/berkeley-db/index.html
Boost           \boost-1.53.0          http://www.boost.org/users/download/
miniupnpc       \miniupnpc-1.8         http://miniupnp.tuxfamily.org/files/

Their licenses:

	OpenSSL        Old BSD license with the problematic advertising requirement
	Berkeley DB    New BSD license with additional requirement that linked software must be free open source
	Boost          MIT-like license
	miniupnpc      New (3-clause) BSD license

Versions used in this release:

	OpenSSL      1.0.1e
	Berkeley DB  4.8.30.NC
	Boost        1.53.0
	miniupnpc    1.8


OpenSSL
-------
MSYS shell:

un-tar sources with MSYS 'tar xfz' to avoid issue with symlinks (OpenSSL ticket 2377)
change 'MAKE' env. variable from 'C:\MinGW32\bin\mingw32-make.exe' to '/c/MinGW32/bin/mingw32-make.exe'

	cd /c/openssl-1.0.1e
	./config
	make

LevelDB
-------
MSYS shell:
	cd /c/Feathercoin-0.8.5/leveldb
	TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a
	
Berkeley DB
-----------
MSYS shell:

	cd /c/db-4.8.30.NC-mgw/build_unix
	../dist/configure --disable-replication --enable-mingw --enable-cxx
	
	Open build_unix/db.h in a text editor and find the following line
	typedef pthread_t db_threadid_t;

	Replace the line with the following
	typedef u_int32_t db_threadid_t;

	Now go back to the MSYS shell and run
	make

Boost
-----
DOS prompt:

	cd /c/deps/boost_1_53_0/
	bootstrap.bat mingw
	b2 --toolset=gcc --build-dir=%BOOST_ROOT%\build --build-type=minimal stage link=static runtime-link=static threading=multi variant=release -a -j 4 --with-filesystem --with-program_options --with-system --with-thread

MiniUPnPc
---------
UPnP support is optional, make with `USE_UPNP=` to disable it.

MSYS shell:

	cd /c/miniupnpc-1.8
	make -f Makefile.mingw
	mkdir miniupnpc
	cp *.h miniupnpc/

Feathercoin
-------
DOS prompt:

	cd \feathercoin\src
	mingw32-make -f makefile.mingw
	strip feathercoind.exe
