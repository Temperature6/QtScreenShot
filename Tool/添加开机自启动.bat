@echo off
setlocal EnableDelayedExpansion
set "CurrentPath=!cd!"
set "exeName=\ScreenShot.exe"
set "exePath=%CurrentPath%%exeName%"
start "" .\RegAutoRun.exe "-add" "ScreenShotBGRun" %exePath% -background
pause