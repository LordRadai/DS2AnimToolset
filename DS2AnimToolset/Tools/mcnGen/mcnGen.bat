@echo off
rem mcnGen.bat - run the whole xml -> mcn rebuild for one network.
rem Usage: mcnGen.bat [input.xml] [config.ini]
rem   input.xml   network export to rebuild (default: INPUT_XML from the config)
rem   config.ini  settings file (default: rebuild_config.ini next to this file)
setlocal EnableExtensions EnableDelayedExpansion

set "TOOLS=%~dp0"
set "ARG_XML=%~f1"
if "%~1"=="" set "ARG_XML="
set "CFG=%~2"
if "%CFG%"=="" set "CFG=%TOOLS%config.ini"
if not exist "%CFG%" (echo Config file not found: "%CFG%" & exit /b 1)

rem ---- read key=value pairs (lines starting with ; are comments)
set "INPUT_XML=" & set "CONNECT=" & set "PYTHON=python" & set "CP_CONFIG=" & set "CLEAN=1"
for /f "usebackq eol=; tokens=1,* delims==" %%A in ("%CFG%") do (
    if not "%%A"=="" set "%%A=%%B"
)
if not "%ARG_XML%"=="" set "INPUT_XML=%ARG_XML%"
if "%INPUT_XML%"=="" (echo No input: pass the .xml as the first argument or set INPUT_XML in "%CFG%" & exit /b 1)
if not exist "%INPUT_XML%" (echo Input not found: "%INPUT_XML%" & exit /b 1)
if not exist "%CONNECT%" (echo morphemeConnect not found: "%CONNECT%" & exit /b 1)

for %%F in ("%INPUT_XML%") do (set "DIR=%%~dpF" & set "NAME=%%~nF")
set "DIR=%DIR:~0,-1%"
rem generation-only files (scripts, logs, diff, round-trip export) go to <project>\build
set "BUILD=%DIR%\build"
set "CPARG="
if not "%CP_CONFIG%"=="" set CPARG=--cp-config "%CP_CONFIG%"

echo === %NAME%  (%DIR%)

rem ---- Connect is single instance: a running copy makes the headless one quit
tasklist /FI "IMAGENAME eq morphemeConnect.exe" 2>nul | find /I "morphemeConnect.exe" >nul
if not errorlevel 1 (echo morphemeConnect is running - close it and run again. & exit /b 1)

if "%CLEAN%"=="1" (
    del /q "%DIR%\%NAME%.mcn" "%BUILD%\%NAME%_paths.lua" "%BUILD%\%NAME%_rebuild.lua" "%BUILD%\%NAME%_stage2.lua" "%BUILD%\%NAME%_stage3.lua" "%BUILD%\roundtrip\%NAME%.xml" 2>nul
)

echo --- 1/5 generating scripts
"%PYTHON%" "%TOOLS%xml2mcn.py" "%INPUT_XML%" %CPARG% --cp-template
if errorlevel 1 (echo xml2mcn failed & exit /b 1)
if not exist "%BUILD%\%NAME%_rebuild.lua" (echo xml2mcn wrote no scripts & exit /b 1)

echo --- 2/5 Connect stage 1
"%CONNECT%" -nogui -script "%BUILD%\%NAME%_rebuild.lua"
call :lastline "%BUILD%\%NAME%_rebuild.log"
if not exist "%DIR%\%NAME%.mcn" (echo Stage 1 did not save %NAME%.mcn - see build\%NAME%_rebuild.log & exit /b 1)

if exist "%BUILD%\%NAME%_stage2.lua" (
    echo --- 3/5 patching the .mcn
    "%PYTHON%" "%TOOLS%xml2mcn.py" "%INPUT_XML%" --inject %CPARG%
    if errorlevel 1 (echo --inject failed & exit /b 1)
    echo --- 4/5 Connect stage 2
    "%CONNECT%" -nogui -script "%BUILD%\%NAME%_stage2.lua"
    call :lastline "%BUILD%\%NAME%_stage2.log"
) else (
    echo --- 3/5 patching the .mcn ^(CP groups and vector ranges^); 4/5 not needed for this network
    "%PYTHON%" "%TOOLS%xml2mcn.py" "%INPUT_XML%" --inject %CPARG%
    if errorlevel 1 (echo --inject failed & exit /b 1)
)

if exist "%BUILD%\%NAME%_stage3.lua" (
    echo --- 4b/5 ActiveStates that list transitions from ActiveStates, then re-export
    "%PYTHON%" "%TOOLS%xml2mcn.py" "%INPUT_XML%" --inject-late
    if errorlevel 1 (echo --inject-late failed & exit /b 1)
    "%CONNECT%" -nogui -script "%BUILD%\%NAME%_stage3.lua"
    call :lastline "%BUILD%\%NAME%_stage3.log"
)

echo --- 5/5 checking
rem DIFF: 0 = round trip identical, 1 = differences, 2 = no round-trip export to compare
set "DIFF=2"
if exist "%BUILD%\roundtrip\%NAME%.xml" (
    "%PYTHON%" "%TOOLS%xmldiff.py" "%INPUT_XML%" "%BUILD%\roundtrip\%NAME%.xml" --paths "%BUILD%\%NAME%_paths.lua" --max 5000 > "%BUILD%\diff_full.txt"
    "%PYTHON%" "%TOOLS%xmldiff.py" "%INPUT_XML%" "%BUILD%\roundtrip\%NAME%.xml" --paths "%BUILD%\%NAME%_paths.lua" --max 0
    if errorlevel 1 (set "DIFF=1") else (set "DIFF=0")
) else (
    echo No round-trip export was written - Connect's export failed, see the stage logs.
)
"%PYTHON%" "%TOOLS%layoutcheck.py" "%DIR%\%NAME%.mcn"

echo --- packing the Connect project
call "%TOOLS%mcnPack.bat" "%DIR%"
set "PACKED=%errorlevel%"
rem build\ moved into the project with the rest
if "%PACKED%"=="0" set "BUILD=%DIR%_project\build"

echo.
if "%PACKED%"=="0" (echo Project:  %DIR%_project\%NAME%.mcn) else (echo Project:  %DIR%\%NAME%.mcn - packing FAILED, see the mcnPack message above)
echo Diff:     %BUILD%\diff_full.txt
echo Missing CP config entries: %BUILD%\%NAME%_cp_config_missing.log

rem ---- warn with a message box when the rebuilt network does not match the export (waits for OK)
if "%DIFF%"=="1" call :warn "%NAME%: the rebuilt network differs from the export. See %BUILD%\diff_full.txt"
if "%DIFF%"=="2" call :warn "%NAME%: Connect wrote no round-trip export, so the rebuild could not be checked. See the stage logs in %BUILD%"
if not "%PACKED%"=="0" exit /b 1
exit /b 0

:warn
echo WARNING: %~1
set "WARN_TEXT=%~1"
rem MCNGEN_NOPOPUP=1 skips the box (unattended runs)
if "%MCNGEN_NOPOPUP%"=="1" exit /b 0
powershell -NoProfile -Command "Add-Type -AssemblyName System.Windows.Forms; [void][System.Windows.Forms.MessageBox]::Show($env:WARN_TEXT, 'mcnGen', 'OK', 'Warning')"
exit /b 0

:lastline
rem print the DONE / export lines of a stage log
if exist "%~1" (findstr /B /C:"DONE" /C:"export" /C:"FAIL" /C:"NIL" "%~1") else (echo log %~1 missing)
exit /b 0
