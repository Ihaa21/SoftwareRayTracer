@echo off

set CodeDir=W:\pixelscratcher\code
set OutputDir=W:\pixelscratcher\build_win32

set CommonCompilerFlags=-O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4127 -wd4201 -wd4100 -wd4189 -wd4505 -Z7 -FC
set CommonCompilerFlags=-DPIXELSCRATCHER_INTERNAL=1 -DPIXELSCRATCHER_WINDOWS=1 %CommonCompilerFlags%
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib Winmm.lib opengl32.lib

IF NOT EXIST %OutputDir% mkdir %OutputDir%

pushd %OutputDir%

del *.pdb > NUL 2> NUL

REM 64-bit build
echo WAITING FOR PDB > lock.tmp
cl %CommonCompilerFlags% %CodeDir%\pixelscratcher.cpp -Fmpixelscratcher.map -LD /link %CommonLinkerFlags% -incremental:no -opt:ref -PDB:pixelscratcher_%random%.pdb -EXPORT:GameUpdateAndRender
del lock.tmp
cl %CommonCompilerFlags% %CodeDir%\win32_pixelscratcher.cpp -Fmwin32_pixelscratcher.map /link %CommonLinkerFlags%

popd
