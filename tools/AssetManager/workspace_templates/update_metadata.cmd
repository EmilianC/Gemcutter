@echo off

title Updating Metadata
call "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/asset_manager.exe" --update
if errorlevel 1 (
   title Update Failure
   pause
   goto :eof
)

title Done!
echo Done!
goto :eof
