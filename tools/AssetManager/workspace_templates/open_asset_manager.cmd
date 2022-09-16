@echo off

:: Prefer the Release version if it exists.
if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" (
    start "" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe"
    goto :EOF
)

if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe" (
    start "" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe"
    goto :EOF
)

start "" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/asset_manager.exe"
