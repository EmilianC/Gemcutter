@echo off

title Packing Assets
call "${ASSET_MANAGER_BINARY_DIR}/Release/asset_manager.exe" --pack
if errorlevel 1 (
   title Assets Failure
   pause
   goto :eof
)

title Done!
echo Done!
goto :eof
