@echo off
setlocal enableextensions

set "SOURCE=%~1"
set "DEST=%~2"

if "%SOURCE%"=="" (
    echo Usage: %~nx0 ^<SourceFolder^> ^<DestinationFolder^>
    exit /b 1
)

if "%DEST%"=="" (
    echo Usage: %~nx0 ^<SourceFolder^> ^<DestinationFolder^>
    exit /b 1
)

if not exist "%DEST%" (
    mkdir "%DEST%"
)

:: Recursively search for .mcn files and copy them to the target folder directly
for /r "%SOURCE%" %%F in (*.mcn) do (
    if exist "%%F" (
        copy /y "%%F" "%DEST%\"
    )
)

echo Done!