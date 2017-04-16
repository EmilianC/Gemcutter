@echo off

call :BuildConfiguration Debug || goto :eof
call :BuildConfiguration Release || goto :eof
call :BuildConfiguration ReleaseWithExceptions || goto :eof

title Done!
echo Done!
goto :eof

:BuildConfiguration
title Building %1...
echo Building %1...
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSbuild.exe" /t:Build /p:Configuration=%1 /property:Platform=Win32 "Jewel3D.sln"
if errorlevel 1 (
   title %1 Build Failure.
   echo %1 Build Failure.
   pause
)
goto :eof
