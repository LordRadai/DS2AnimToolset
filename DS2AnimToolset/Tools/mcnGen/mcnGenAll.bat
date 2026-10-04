@echo off
rem mcnGenAll.bat - run mcnGen.bat on every cXXXX\cXXXX.xml inside a folder.
rem Usage: mcnGenAll.bat <folder> [config.ini]
rem   folder      folder holding one cXXXX subfolder per character (e.g. ...\bin\Export)
rem   config.ini  passed on to mcnGen.bat (default: rebuild_config.ini next to it)
setlocal EnableExtensions EnableDelayedExpansion

set "TOOLS=%~dp0"
set "ROOT=%~f1"
if "%~1"=="" (echo Usage: mcnGenAll.bat ^<folder^> [config.ini] & exit /b 1)
if not exist "%ROOT%\" (echo Folder not found: "%ROOT%" & exit /b 1)

set /a OK=0, FAILED=0, SKIPPED=0
set "FAILLIST="
for /d %%D in ("%ROOT%\c*") do (
    set "C=%%~nxD"
    rem only cXXXX folders: c followed by four digits
    echo !C!| findstr /r /x "c[0-9][0-9][0-9][0-9]" >nul
    if not errorlevel 1 (
        if exist "%%D\!C!.xml" (
            call "%TOOLS%mcnGen.bat" "%%D\!C!.xml" %2
            if errorlevel 1 (set /a FAILED+=1 & set "FAILLIST=!FAILLIST! !C!") else (set /a OK+=1)
        ) else (
            echo === !C!: no !C!.xml, skipped
            set /a SKIPPED+=1
        )
    )
)

echo.
echo ===== done: %OK% built, %FAILED% failed, %SKIPPED% skipped
if not "%FAILLIST%"=="" echo failed:%FAILLIST%
if %FAILED% gtr 0 exit /b 1
exit /b 0
