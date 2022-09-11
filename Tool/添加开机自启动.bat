@echo off
setlocal EnableDelayedExpansion
set "CurrentPath=!cd!"
set "exeName=\BGRun.vbs"
set "exePath=%CurrentPath%%exeName%"
start "" .\RegAutoRun.exe "-add" "ScreenShotBGRun" %exePath%
pause