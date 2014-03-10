#Build Instructions for Feathercoin in Visual Studio 2013

----------

I have created a VS2013 solution and project files to build FeathercoinD and Feathercoin-Qt.  To try this out, the first thing you need to do is install VS2013, along with the latest updates, and then get all the dependecies downloaded, arranged in the expected file structure, and then built.  If it all works, you will have 32 and 64 bit versions in debug and release mode of both the daemon and the Qt exe.

Good Luck!


## Building All of the Feathercoin Dependencies in Visual Studio 2013 ##

To build all of the required dependencies, you first must download them.  Here they are:

- [openssl-1.0.1e](http://www.openssl.org/source/openssl-1.0.1e.tar.gz)
- [Qt-5.2.1](http://download.qt-project.org/official_releases/qt/5.2/5.2.1/single/qt-everywhere-opensource-src-5.2.1.7z)
- [miniupnp-1.8](http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.8.20131209.tar.gz)
- [qrencode-unknown version](http://qrencode-win32.googlecode.com/archive/681f2ea7a41f919486d9932b3352a2e6920e1cb9.zip)
- [boost-1.55](http://softlayer-dal.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.zip)
- [BerkeleyDB-4.7.25](http://download.oracle.com/berkeley-db/db-4.7.25.NC.tar.gz)

If you want to use alternative versions of these packages, then my batch and project files may not work.  

I use [7-zip](http://www.7-zip.org/download.html) to extract archives on windows:


Make sure you have perl, python and nasm installed and ***in your path***:

- [ActivePerl](http://www.activestate.com/activeperl/downloads)
- [Python](http://www.python.org/download/releases/)
- [nasm](http://sourceforge.net/projects/nasm/files/latest/download)


Since this guide is written for building with Visual Studio 2013, please make sure that is installed with all the latest updates.  As of this writing (March 6th, 2014), there is only Update 1 for VS2013 which is available [here](http://download.microsoft.com/download/8/2/6/826E264A-729E-414A-9E67-729923083310/VSU1/VS2013.1.exe).

Everything is built using the v120 toolset so the resulting executables might not run on Windows XP.  There is another toolset option that allows you to maintain compatibility with XP.  

If you want to build everything using my ***unmodified*** batch files, then create this directory:

**C:\MyProjects\BitcoinDeps**

And put all the extracted download folders in the BitcoinDeps folder.  Most of them will have to be renamed to match what the build batch file is expecting.  For example, the Qt folder needs to be placed in the BitcoinDeps directory twice, once with the name qtdir32 and another called qtdir64.

Also, copy the build-helpers directory to C:\MyProjects\BitcoinDeps.  Here is what the MyProjects folder should look like once you are ready to build, including any *coin folders you may have:

    C:\MyProjects\
        BitcoinDeps\
            boost_1_55_0\
            build-helpers\
            db-4.7.25.NC\
            miniupnpc\
            openssl-1.0.1e\
            qrencode-win32\
            qtdir32\
            qtdir64\
        bitcoin-master-0.8.6\
        Feathercoin-master-0.8.6\
        litecoin-master-0.8.6.2\


####Am I Finally Ready To Start Building Something??####

Let's make sure.  Have you verified the following:

- Your path contains all the locations for nasm, perl, and python.  Bring up a command prompt and try all three.
- You installed VS2013 to the default location on the C drive.
- You created folders in BitcoinDeps that match the folder names above and removed extra folder layers.  For example, the contents of miniupnpc should contain real stuff, not just a single folder--if you don't remove those extra "layers", the scripts will fail.
- You copied the build-helpers folder from MSVC and placed it in BitcoinDeps.

If you think it all looks good, then open up the build-helpers directory in BitcoinDeps and you can begin executing the batch files. 

**Always execute buildopenssl.bat first** because the Qt builds need it.  And don't start building Qt before OpenSsl is finished!  If anything goes wrong with a Qt build, always start over with a clean qtdir32 or qtdir64 folder. I make an extra copy after downloading.

The other build files can be run in parallel, including both of the qt builds.  By the way, the Qt builds can take several hours to complete.

Some of the dependencies are built using Visual Studio project files that I created.  If something goes wrong with one of these, you can dig into it by opening up the project file in Visual Studio 2013 and trying to build interactively.


----------
###Building the Daemon and Qt Exe###

Hopefully, if you have made it this far, you will know what to do here.  Open up the solution and build it in VS2013.  By the way, you will also need code changes from my fork in order for the code to build and run correctly.  

----------



I've tried to make this guide as complete as possible and I hope it helps you. If you find any mistakes or omissions, please let me know by posting in my thread at [bitcointalk](https://bitcointalk.org/index.php?topic=349094).

Thank you,<br/>Claire
