@echo off

call :BuildConfiguration Debug Win32 || goto :eof
call :BuildConfiguration Release Win32 || goto :eof
call :BuildConfiguration Debug "Any CPU" || goto :eof
call :BuildConfiguration Release "Any CPU" || goto :eof
call :BuildConfiguration ReleaseWithExceptions Win32 || goto :eof

title Done!
echo Done!
goto :eof

:BuildConfiguration
title Building %1 - %2
echo ^>^>^>^>^>^> Building %1 - %2 ^<^<^<^<^<^<
"%ProgramFiles(x86)%\MSBuild\14.0\Bin\MSbuild.exe" /t:Build /p:Configuration=%1 /property:Platform=%2 "Jewel3D.sln"
if errorlevel 1 (
   title Build Failure %1 - %2
   echo ^>^>^>^>^>^> Build Failure %1 - %2 ^<^<^<^<^<^<
   pause
)
goto :eof
