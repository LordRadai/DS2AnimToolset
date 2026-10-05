@echo off
rem mcnPack.bat - move a rebuilt character into a clean morphemeConnect project folder.
rem Usage: mcnPack.bat <input folder> [output folder]
rem   input folder   a character folder mcnGen.bat has built (e.g. ...\Export\c0001)
rem   output folder  where the project goes (default: <input folder>_project)
rem The files are moved, not copied: the input folder keeps the export XMLs, .mrarig rigs, names table and
rem build\, so run the decompiler export again before rebuilding the same character.
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
move /y "%SRC%\%NAME%.mcn" "%DST%\" >nul || goto :copyfail
move /y "%SRC%\%NAME%.mcp" "%DST%\" >nul || goto :copyfail
move /y "%SRC%\*.mcarig" "%DST%\" >nul || goto :copyfail
move /y "%SRC%\*.mcskin" "%DST%\" >nul || goto :copyfail
for %%D in (motion_xmd model_xmd morphemeMarkup) do (
    robocopy "%SRC%\%%D" "%DST%\%%D" /E /MOVE /NFL /NDL /NJH /NJS /NP >nul
    if errorlevel 8 goto :copyfail
)

set /a RIGS=0
for %%R in ("%DST%\*.mcarig") do set /a RIGS+=1
echo moved %NAME%.mcn, %NAME%.mcp, %RIGS% rig(s) with skins, motion_xmd, model_xmd, morphemeMarkup
exit /b 0

:copyfail
echo Move failed - see the message above.
exit /b 1
