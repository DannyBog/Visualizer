@echo off

set "app=Visualizer"

if "%1" equ "debug" (
	set "cl=/MTd /Od /D_DEBUG /Zi /RTC1 /Fd%app%.pdb /fsanitize=address"
	set "link=/DEBUG"
) else (
	set "cl=/GL /O1 /DNDEBUG /GS-"
	set "link=/LTCG /OPT:REF /OPT:ICF libvcruntime.lib"
)

set "warnings=/wd4100 /wd4706"

if not exist "%~dp0..\output" mkdir "%~dp0..\output"

pushd "%~dp0..\output"
rc /nologo /fo "%app%.res" "..\res\%app%.rc"
cl /nologo /WX /W4 %warnings% /MP "..\src\main.c" /Fe"%app%" "%app%.res" /link /INCREMENTAL:NO /MANIFEST:EMBED /MANIFESTINPUT:"..\res\%app%.manifest" /SUBSYSTEM:WINDOWS /FIXED /merge:_RDATA=.rdata

del *.obj *.res >nul
popd