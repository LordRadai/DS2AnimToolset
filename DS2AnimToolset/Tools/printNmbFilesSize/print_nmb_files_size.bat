@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ---- Validate input ----
if "%~1"=="" (
    echo Usage: %~nx0 TargetFolder [Extension]
    echo Example: %~nx0 "D:\Projects" nmb
    exit /b 1
)

set "TARGET_DIR=%~1"
set "EXTENSION=nmb"

REM Ensure TARGET_DIR does not end with a backslash
if "%TARGET_DIR:~-1%"=="\" set "TARGET_DIR=%TARGET_DIR:~0,-1%"

REM ---- Recursive scan ----
for /R "%TARGET_DIR%" %%F in (*.%EXTENSION%) do (
    set "FULL=%%~fF"
    REM Remove the target folder prefix and leading backslash
    set "REL=!FULL:%TARGET_DIR%\=!"
    echo !REL! ^| %%~zF bytes
)

endlocal
