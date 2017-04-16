@echo off

pushd Engine\bin\Debug_Win32
call UnitTests.exe
popd

pushd Engine\bin\ReleaseWithExceptions_Win32
call UnitTests.exe
popd

title Done Testing!
echo Done Testing!