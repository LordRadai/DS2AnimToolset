@echo off
rem Converts every .xmd in a folder with xmd2Ap2.py.
rem Output name keeps only aXX_YY_ZZ (the first three "_" parts), e.g.
rem   a00_00_0000_Idle.xmd -> a00_00_0000.ani
rem Usage: xmd2Ap2_batch.bat <input folder> [output folder] [--target le64|le32|be32] [--up-bones ...]
rem Output folder defaults to the input folder. Extra arguments are passed to xmd2Ap2.py.
setlocal EnableDelayedExpansion

if "%~1"=="" (
    echo Usage: %~nx0 ^<input folder^> [output folder] [xmd2Ap2.py options]
    exit /b 1
)

set "HERE=%~dp0"
set "IN=%~f1"
set "OUT=%IN%"
shift
if not "%~1"=="" (
    set "ARG=%~1"
    if not "!ARG:~0,2!"=="--" (
        set "OUT=%~f1"
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
if not exist "%OUT%" mkdir "%OUT%"
set /a OK=0, FAIL=0

for %%f in ("%IN%\*.xmd") do (
    for /f "tokens=1-3 delims=_" %%a in ("%%~nf") do set "NAME=%%a_%%b_%%c"
    echo %%~nxf -^> !NAME!.ani
    python "%HERE%xmd2Ap2.py" "%%~ff" "%OUT%\!NAME!.ani" !OPTS!
    if errorlevel 1 (set /a FAIL+=1) else (set /a OK+=1)
)

echo Done: !OK! converted, !FAIL! failed.
endlocal
