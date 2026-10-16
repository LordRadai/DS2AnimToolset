@echo off
rem Runs xmd2Ap2_batch.bat over every cXXXX\motion_xmd folder in the input folder.
rem   InputFolder\c0000\motion_xmd\*.xmd -> OutputRoot\Motion\c0000\aXX_YY_ZZ.ani
rem Usage: xmd2Ap2_chr.bat InputFolder [OutputRoot] [--target le64|le32|be32] [--up-bones ...]
rem OutputRoot defaults to the input folder. Extra arguments are passed to xmd2Ap2.py.
setlocal EnableDelayedExpansion

if "%~1"=="" (
    echo Usage: %~nx0 InputFolder [OutputRoot] [xmd2Ap2.py options]
    exit /b 1
)

set "HERE=%~dp0"
set "IN=%~f1"
set "ROOT=%IN%"
shift
if not "%~1"=="" (
    set "ARG=%~1"
    if not "!ARG:~0,2!"=="--" (
        set "ROOT=%~f1"
        shift
    )
)

set "OPTS="
:collect
if "%~1"=="" goto run
set "OPTS=!OPTS! %1"
shift
goto collect

:run
set /a DIRS=0
for /d %%d in ("%IN%\c*") do (
    echo %%~nxd| findstr /r /x /i "c[0-9][0-9][0-9][0-9]" >nul
    if not errorlevel 1 if exist "%%~fd\motion_xmd\" (
        echo.
        echo === %%~nxd ===
        call "%HERE%xmd2Ap2_batch.bat" "%%~fd\motion_xmd" "%ROOT%\Motion\%%~nxd" !OPTS!
        set /a DIRS+=1
    )
)

echo.
echo Processed !DIRS! character folders.
endlocal
