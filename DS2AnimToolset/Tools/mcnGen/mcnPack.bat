@echo off
rem mcnPack.bat - copy a rebuilt character into a clean morphemeConnect project folder.
rem Usage: mcnPack.bat <input folder> [output folder]
rem   input folder   a character folder mcnGen.bat has built (e.g. ...\Export\c0001)
rem   output folder  where the project goes (default: <input folder>_project)
rem The project gets only what Connect needs: <chr>.mcn, <chr>.mcp, one .mcarig and .mcskin per animation
rem set, and the motion_xmd, model_xmd and morphemeMarkup folders. All paths in the project are
rem $(RootDir)-relative, so the folder can be moved anywhere.
setlocal EnableExtensions

if "%~1"=="" (echo Usage: mcnPack.bat ^<input folder^> [output folder] & exit /b 1)
set "SRC=%~f1"
if not exist "%SRC%\" (echo Input folder not found: "%SRC%" & exit /b 1)
set "DST=%~f2"
if "%~2"=="" set "DST=%SRC%_project"
for %%F in ("%SRC%") do set "NAME=%%~nxF"

rem ---- everything the project needs must exist
set "MISSING="
for %%X in (mcn mcp) do if not exist "%SRC%\%NAME%.%%X" set "MISSING=%MISSING% %NAME%.%%X"
if not exist "%SRC%\*.mcarig" set "MISSING=%MISSING% *.mcarig"
if not exist "%SRC%\*.mcskin" set "MISSING=%MISSING% *.mcskin"
for %%D in (motion_xmd model_xmd morphemeMarkup) do if not exist "%SRC%\%%D\" set "MISSING=%MISSING% %%D\"
if defined MISSING (echo Missing in "%SRC%":%MISSING% & echo Run mcnGen.bat on this character first. & exit /b 1)

if /i "%DST%"=="%SRC%" (echo The output folder must differ from the input folder. & exit /b 1)
if not exist "%DST%\" mkdir "%DST%" || (echo Cannot create "%DST%" & exit /b 1)

echo === %NAME%: "%SRC%" -^> "%DST%"
copy /y "%SRC%\%NAME%.mcn" "%DST%\" >nul || goto :copyfail
copy /y "%SRC%\%NAME%.mcp" "%DST%\" >nul || goto :copyfail
copy /y "%SRC%\*.mcarig" "%DST%\" >nul || goto :copyfail
copy /y "%SRC%\*.mcskin" "%DST%\" >nul || goto :copyfail
for %%D in (motion_xmd model_xmd morphemeMarkup) do (
    robocopy "%SRC%\%%D" "%DST%\%%D" /E /NFL /NDL /NJH /NJS /NP >nul
    if errorlevel 8 goto :copyfail
)

for /f %%N in ('dir /b "%DST%\*.mcarig" ^| find /c /v ""') do set "RIGS=%%N"
echo copied %NAME%.mcn, %NAME%.mcp, %RIGS% rig(s) with skins, motion_xmd, model_xmd, morphemeMarkup
exit /b 0

:copyfail
echo Copy failed - see the message above.
exit /b 1
