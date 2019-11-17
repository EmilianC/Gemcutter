@echo off

:: Check for permissions.
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    ECHO INSTALL.cmd must be run as admin.
    Pause
    goto :eof
)

ECHO -----------------------------------------------------------------------------------------
ECHO                               - Installing Jewel3D Framework -
ECHO -----------------------------------------------------------------------------------------

ECHO 1) Setting Environment Variable.
SETX Jewel3D_Path "%~dp0\" -m > nul
ECHO Done!
ECHO.

ECHO 2) Installing Template Project.
XCOPY "%~dp0\Resources\Jewel3D Project.zip" "%USERPROFILE%\Documents\Visual Studio 2019\Templates\ProjectTemplates\" /Y /V > nul
XCOPY "%~dp0\Resources\Jewel3D Encoder.zip" "%USERPROFILE%\Documents\Visual Studio 2019\Templates\ProjectTemplates\" /Y /V > nul
ECHO Done!
ECHO.

ECHO -----------------------------------------------------------------------------------------
ECHO     * A restart cycle of your computer might be required to register these changes *
ECHO -----------------------------------------------------------------------------------------
Pause