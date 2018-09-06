@echo off

if not "%1" == "-r" (
	call :BuildConfiguration Debug Win32 || goto :eof
	call :BuildConfiguration Debug "Any CPU" || goto :eof
)
if not "%1" == "-d" (
	call :BuildConfiguration Release Win32 || goto :eof
	call :BuildConfiguration Release "Any CPU" || goto :eof
)

title Done!
echo Done!
goto :eof

:BuildConfiguration
title Building %1 - %2
echo ^>^>^>^>^>^> Building %1 - %2 ^<^<^<^<^<^<
"%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSbuild.exe" /v:m /t:Build /p:Configuration=%1 /property:Platform=%2 "Jewel3D.sln"
if errorlevel 1 (
   title Build Failure %1 - %2
   echo ^>^>^>^>^>^> Build Failure %1 - %2 ^<^<^<^<^<^<
   pause
)
goto :eof
