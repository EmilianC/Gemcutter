@echo off

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"=""
    echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------

ECHO -------------------------------------------------------------------------------
ECHO                        - Installing Jewel3D Engine -
ECHO -------------------------------------------------------------------------------
::
ECHO 1) Setting Environment Variable.
SETX Jewel3D_Path "%CD%\\" -m > nul
::
ECHO 2) Installing Template Project.
XCOPY "Resources\Jewel3D Project.zip" "%USERPROFILE%\Documents\Visual Studio 2015\Templates\ProjectTemplates\" /Y /V > nul
::
ECHO Done!
ECHO -------------------------------------------------------------------------------
ECHO     * A restart cycle of Visual Studio or your computer might be required *
ECHO -------------------------------------------------------------------------------
Pause