@ECHO ON
cd C:\MyProjects\BitcoinDeps\db-4.7.25.NC\build_windows
if %errorlevel% NEQ 0 goto ERRORCLEANUP
set VisualStudioVersion=12.0
copy C:\MyProjects\BitcoinDeps\build-helpers\db_static.vcxproj . /Y
if %errorlevel% NEQ 0 goto ERRORCLEANUP
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
msbuild.exe  db_static.vcxproj /t:clean /p:Configuration="Release x86" /p:Platform="Win32"
msbuild.exe  db_static.vcxproj /t:clean /p:Configuration="Debug x86" /p:Platform="Win32"
msbuild.exe  db_static.vcxproj /p:Configuration="Release x86" /p:Platform="Win32"
msbuild.exe  db_static.vcxproj /p:Configuration="Debug x86" /p:Platform="Win32"
REM
REM now clean and build 64 bit
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
msbuild.exe  db_static.vcxproj /t:clean /p:Configuration="Release AMD64" /p:Platform="x64"
msbuild.exe  db_static.vcxproj /t:clean /p:Configuration="Debug AMD64" /p:Platform="x64"
msbuild.exe  db_static.vcxproj /p:Configuration="Release AMD64" /p:Platform="x64"
msbuild.exe  db_static.vcxproj /p:Configuration="Debug AMD64" /p:Platform="x64"
echo All finished!
pause
goto EOF
:ERRORCLEANUP
echo Something went wrong, please check the directories in this batch file!
pause
:EOF