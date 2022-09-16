@echo off

if "%1" == "" goto :default

title Packing Assets [%1]
call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/%1/asset_manager.exe" --pack
goto :end

:default
:: Prefer the Release version if it exists.
if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" (
    title Packing Assets [Release]
    call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" --pack
    goto :end
)

if exist "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe" (
    title Packing Assets [RelWithDebInfo]
    call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/asset_manager.exe" --pack
    goto :end
)

title Packing Assets [Debug]
call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/asset_manager.exe" --pack

:end
if errorlevel 1 (
   title Packing failed!
   pause
   goto :eof
)

title Packing completed!
echo Packing completed!
goto :eof
