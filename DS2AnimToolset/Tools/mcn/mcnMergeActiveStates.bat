@echo off
setlocal EnableDelayedExpansion
rem Run mcnMergeActiveStates.py on every .mcn in a folder and its subfolders.
rem Usage: mcnMergeActiveStates.bat <folder> [--write] [--sm NAME] [--any-dest]
rem Without --write every file is only dry-run (plan printed, nothing changed).

if "%~1"=="" (
    echo Usage: %~nx0 ^<folder^> [--write] [--sm NAME] [--any-dest]
    exit /b 1
)
if not exist "%~1\" (
    echo Folder not found: %~1
    exit /b 1
)

set "PY=python"
where python >nul 2>nul || set "PY=py -3"
set "SCRIPT=%~dp0mcnMergeActiveStates.py"
set "ROOT=%~1"
shift
set "ARGS="
:args
if not "%~1"=="" (
    set "ARGS=!ARGS! %1"
    shift
    goto args
)

set /a N=0
set /a FAIL=0
for /r "%ROOT%" %%F in (*.mcn) do (
    echo.
    echo === %%F
    %PY% "%SCRIPT%" "%%F" !ARGS!
    if errorlevel 1 set /a FAIL+=1
    set /a N+=1
)
echo.
echo Processed !N! file(s), !FAIL! failed.
endlocal
