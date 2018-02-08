Feathercoin 0.13.1
==================

Copyright (c) 2009-2018 Feathercoin Developers

Setup
---------------------

[Feathercoin Core] is the original Feathercoin client and it builds the backbone of the network. However, it downloads and stores the entire history of Feathercoin transactions (which is currently several GBs); depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more. Thankfully you only have to do this once. 

Binary downloads are avilable here : https://www.feathercoin.com/

### Download the .deb file or install the zxing binaries available after installing the Feathercoin PPA  

(http://forum.feathercoin.com/topic/8327/guide-feathercoin-wallet-ppa-and-binaries-on-ubuntu-and-debian-linux)  

As Super user (or sudo) create a file named **OpenSuse.list** in the directory 

    /etc/apt/sources.list.d 

For your OS, copy the command into /etc/apt/sources.list.d and save it.  
    
    deb http://download.opensuse.org/repositories/home:/wellenreiter01/xUbuntu_16.04 ./

download the repository key:
    wget http://download.opensuse.org/repositories/home:/wellenreiter01/Debian_7.0/Release.key    
   
Then add the key to your system:

    sudo apt-key add Release.key

Running
---------------------
The following are some helpful notes on how to run Feathercoin on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/Feathercoin-qt` (GUI) or
- `bin/Feathercoind` (headless)

### Windows

Unpack the files into a directory, and then run Feathercoin-qt.exe.

Building
---------------------
The following are developer notes on how to build Bitcoin on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [OS X Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The Bitcoin repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Multiwallet Qt Development](multiwallet-qt.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://dev.visucore.com/bitcoin/doxygen/)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [Unit Tests](unit-tests.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Shared Libraries](shared-libraries.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)

License
---------------------
Distributed under the [MIT software license](http://www.opensource.org/licenses/mit-license.php).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
