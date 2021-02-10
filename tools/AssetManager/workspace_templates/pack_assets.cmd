@echo off

title Packing Assets
call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" --pack
if errorlevel 1 (
   title Assets Failure
   pause
   goto :eof
)

title Done!
echo Done!
goto :eof
