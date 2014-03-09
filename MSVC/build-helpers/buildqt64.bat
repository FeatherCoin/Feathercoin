@ECHO ON
cd C:\MyProjects\BitcoinDeps\qtdir64
if %errorlevel% NEQ 0 goto ERRORCLEANUP
SET OLDPATH=%PATH%
set VisualStudioVersion=12.0
REM first change the debug compiler options
perl -pi.bak -e "s#QMAKE_CFLAGS_RELEASE    = -O2 -MD#QMAKE_CFLAGS_RELEASE    = -O2 -MT#g;" qtbase\mkspecs\win32-msvc2013\qmake.conf
perl -pi.bak -e "s#QMAKE_CFLAGS_DEBUG      = -Zi -MDd#QMAKE_CFLAGS_DEBUG      = -Zi -MTd#g;" qtbase\mkspecs\win32-msvc2013\qmake.conf
REM Now do 64 bit by setting environment to MSVC 64 bit
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
call configure -debug-and-release -openssl-linked -opensource -confirm-license -platform win32-msvc2013 -nomake examples -nomake tests -static -I \MyProjects\BitcoinDeps\openssl-1.0.1e\inc64 -L \MyProjects\BitcoinDeps\openssl-1.0.1e\out64.dbg -L \MyProjects\BitcoinDeps\openssl-1.0.1e\out64 -l gdi32 -no-opengl -qt-zlib -qt-libpng -qt-libjpeg
nmake 
REM put back the path
set PATH=%OLDPATH%
echo All finished!
pause
goto EOF
:ERRORCLEANUP
echo Something went wrong, please check the directories in this batch file!
pause
:EOF