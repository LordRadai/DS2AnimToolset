@echo off
rem mcnUpdateCp.bat - apply the CP settings file (cp_config.json) to already built .mcn files, without rebuilding.
rem Usage: mcnUpdateCp.bat <export folder> [projects folder] [config.ini]
rem   export folder    folder with one cXXXX subfolder per character, each holding cXXXX.xml (the decompiler export)
rem   projects folder  folder with the cXXXX project folders to update (default: <export folder>\cXXXX_project,
rem                    or <export folder>\cXXXX when it was never packed)
rem   config.ini       settings file (default: config.ini next to this file); CP_CONFIG picks the CP settings file
rem For every character: groups and vector ranges are written into the .mcn, then Connect sets float/int ranges and
rem defaults (build\cXXXX_cparams.lua in the project) and saves. Nothing else in the .mcn or the .mcp changes.
setlocal EnableExtensions EnableDelayedExpansion

set "TOOLS=%~dp0"
if "%~1"=="" (echo Usage: mcnUpdateCp.bat ^<export folder^> [projects folder] [config.ini] & exit /b 1)
set "ROOT=%~f1"
if not exist "%ROOT%\" (echo Export folder not found: "%ROOT%" & exit /b 1)
set "PROJECTS="
if not "%~2"=="" set "PROJECTS=%~f2"
set "CFG=%~3"
if "%CFG%"=="" set "CFG=%TOOLS%config.ini"
if not exist "%CFG%" (echo Config file not found: "%CFG%" & exit /b 1)

set "CONNECT=" & set "PYTHON=python" & set "CP_CONFIG="
for /f "usebackq eol=; tokens=1,* delims==" %%A in ("%CFG%") do (
    if not "%%A"=="" set "%%A=%%B"
)
if not exist "%CONNECT%" (echo morphemeConnect not found: "%CONNECT%" & exit /b 1)
set "CPARG="
if not "%CP_CONFIG%"=="" set CPARG=--cp-config "%CP_CONFIG%"

rem ---- Connect is single instance: a running copy makes the headless one quit
tasklist /FI "IMAGENAME eq morphemeConnect.exe" 2>nul | findstr /I "morphemeConnect.exe" >nul
if not errorlevel 1 (echo morphemeConnect is running - close it and run again. & exit /b 1)

set /a OK=0, FAILED=0, SKIPPED=0
set "FAILLIST="
for /d %%D in ("%ROOT%\c*") do (
    set "C=%%~nxD"
    echo !C!| findstr /r /x "c[0-9][0-9][0-9][0-9]" >nul
    if not errorlevel 1 if exist "%%D\!C!.xml" (
        set "PROJ="
        if defined PROJECTS if exist "!PROJECTS!\!C!\!C!.mcn" set "PROJ=!PROJECTS!\!C!"
        if not defined PROJ if exist "%ROOT%\!C!_project\!C!.mcn" set "PROJ=%ROOT%\!C!_project"
        if not defined PROJ if exist "%%D\!C!.mcn" set "PROJ=%%D"
        if not defined PROJ (
            echo === !C!: no built .mcn found, skipped
            set /a SKIPPED+=1
        ) else (
            echo === !C!: !PROJ!
            "%PYTHON%" "%TOOLS%xml2mcn.py" "%%D\!C!.xml" !CPARG! --cp-only --out-dir "!PROJ!"
            if errorlevel 1 (
                set /a FAILED+=1 & set "FAILLIST=!FAILLIST! !C!"
            ) else (
                "%CONNECT%" -nogui -script "!PROJ!\build\!C!_cparams.lua"
                set "DONE="
                if exist "!PROJ!\build\!C!_cparams.log" for /f "delims=" %%L in ('findstr /B /C:"DONE" /C:"FAIL" /C:"NIL" "!PROJ!\build\!C!_cparams.log"') do (echo %%L & set "DONE=%%L")
                echo !DONE!| findstr /B /C:"DONE failures=0" >nul
                if errorlevel 1 (set /a FAILED+=1 & set "FAILLIST=!FAILLIST! !C!") else (set /a OK+=1)
            )
        )
    )
)

echo.
echo ===== done: %OK% updated, %FAILED% failed, %SKIPPED% skipped
if not "%FAILLIST%"=="" echo failed (see build\cXXXX_cparams.log in the project):%FAILLIST%
if %FAILED% gtr 0 exit /b 1
exit /b 0
