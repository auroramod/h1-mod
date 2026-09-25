@echo off
setlocal

set "psCommand="(new-object -COM 'Shell.Application').BrowseForFolder(0,'Please choose a folder.',0,0).self.path""

for /f "usebackq delims=" %%I in (`powershell %psCommand%`) do set "folder=%%I"

setlocal enabledelayedexpansion
setx AURORAH1_GAME_PATH "!folder!"
endlocal
