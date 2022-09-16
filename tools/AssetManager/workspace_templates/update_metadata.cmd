@echo off

:: Prefer the Release version if it exists.
if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" (
    title Updating Metadata [Release]
    call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" --update
    goto :end
)

if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe" (
    title Updating Metadata [RelWithDebInfo]
    call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe" --update
    goto :end
)

title Updating Metadata [Debug]
call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/asset_manager.exe" --update

:end
if errorlevel 1 (
   title Update failed!
   pause
   goto :eof
)

title Updating complete!
echo Updating complete!
goto :eof
