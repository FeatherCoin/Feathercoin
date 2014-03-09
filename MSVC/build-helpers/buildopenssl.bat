@ECHO ON
SET OLDPATH=%PATH%
cd C:\MyProjects\BitcoinDeps\openssl-1.0.1e
if %errorlevel% NEQ 0 goto ERRORCLEANUP
set VisualStudioVersion=12.0
REM first change the debug compiler options
rem never mind, I'll stick with Zi
rem perl -pi.bak -e "s#/Zi#/Z7#g;" util/pl/VC-32.pl
REM Now do 64 bit by setting environment to MSVC 64 bit
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
REM compile debug mode first
call perl Configure no-asm debug-VC-WIN64A 
call ms\do_win64a
perl -pi.bak -e "s#out32#out64#g;" ms\nt.mak
perl -pi.bak -e "s#tmp32#tmp64#g;" ms\nt.mak
perl -pi.bak -e "s#inc32#inc64#g;" ms\nt.mak
nmake -f ms\nt.mak clean
nmake -f ms\nt.mak 
if %errorlevel% NEQ 0 goto ERRORCLEANUP
REM Now do release mode
call perl Configure no-asm VC-WIN64A 
call ms\do_win64a
perl -pi.bak -e "s#out32#out64#g;" ms\nt.mak
perl -pi.bak -e "s#tmp32#tmp64#g;" ms\nt.mak
perl -pi.bak -e "s#inc32#inc64#g;" ms\nt.mak
nmake -f ms\nt.mak clean
nmake -f ms\nt.mak 
if %errorlevel% NEQ 0 goto ERRORCLEANUP
REM clean up and move files
set PATH=%OLDPATH%
REM
REM Now do 32 bit by setting environment to MSVC 32 bit
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
REM debug 32 first
call perl Configure debug-VC-WIN32 
call ms\do_nasm
nmake -f ms\nt.mak clean
rem removing a compiler flag that causes the compiler to barf on a warning
perl -pi.bak -e "s#-WX##g;" ms\nt.mak
nmake -f ms\nt.mak 
if %errorlevel% NEQ 0 goto ERRORCLEANUP
REM now do release mode
call perl Configure VC-WIN32 
call ms\do_nasm
nmake -f ms\nt.mak clean
nmake -f ms\nt.mak 
if %errorlevel% NEQ 0 goto ERRORCLEANUP
REM put back the path
set PATH=%OLDPATH%
echo All finished!
pause
goto EOF
:ERRORCLEANUP
echo Something went wrong, please check the directories in this batch file!
pause
:EOF
