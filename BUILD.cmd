@echo off
echo Building Debug...
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSbuild.exe" /t:Build /p:Configuration=Debug /property:Platform=Win32 "Jewel3D.sln"
if errorlevel 1 (
   title Building Debug... Failed!
   echo Debug Build Failure.
   pause
   exit %errorlevel%
)
::
title Building Release...
echo Building Release...
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSbuild.exe" /t:Build /p:Configuration=Release /property:Platform=Win32 "Jewel3D.sln"
if errorlevel 1 (
   title Building Release... Failed!
   echo Release Build Failure.
   pause
   exit %errorlevel%
)
::
echo Done!
pause