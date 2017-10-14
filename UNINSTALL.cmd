@echo off

:: Check for permissions.
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    ECHO UNINSTALL.cmd must be run as admin.
    Pause
    Exit
)

ECHO -----------------------------------------------------------------------------------------
ECHO                               - Uninstalling Jewel3D Engine -
ECHO -----------------------------------------------------------------------------------------

ECHO 1) Removing Environment Variable.
set Jewel3D_Path=
ECHO Done!
ECHO.

ECHO 2) Removing Template Project.
del "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Jewel3D Project.zip" > nul
del "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Jewel3D Encoder.zip" > nul
ECHO Done!
ECHO.

ECHO -----------------------------------------------------------------------------------------
ECHO     * A restart cycle of your computer might be required to register these chagnes *
ECHO -----------------------------------------------------------------------------------------
Pause