@echo off

set OutputDir=W:\pixelscratcher\build_win32

set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4127 -wd4201 -wd4100 -wd4189 -wd4505 -Z7 -FC
set CommonCompilerFlags=-DPIXELSCRATCHER_INTERNAL=1 -DPIXELSCRATCHER_WINDOWS=1 %CommonCompilerFlags%
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib Winmm.lib opengl32.lib

IF NOT EXIST %OutputDir% mkdir %OutputDir%

pushd %OutputDir%

del *.pdb > NUL 2> NUL

REM 32-bit build
REM cl %CommonCompilerFlags% win32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
echo WAITING FOR PDB > lock.tmp
cl %CommonCompilerFlags% W:\pixelscratcher\code\pixelscratcher.cpp -Fmpixelscratcher.map -LD /link %CommonLinkerFlags% -incremental:no -opt:ref -PDB:pixelscratcher_%random%.pdb -EXPORT:GameUpdateAndRender -EXPORT:GameGetSoundSamples
del lock.tmp
cl %CommonCompilerFlags% W:\pixelscratcher\code\win32_pixelscratcher.cpp -Fmwin32_pixelscratcher.map /link %CommonLinkerFlags%

popd
