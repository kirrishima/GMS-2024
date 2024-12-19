@ECHO OFF

cd /d C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build
call vcvarsall.bat x86

cd /d C:\Users\Force\Documents\University\3sem\KPO-3-sem\term-project\GMS-2024\Debug
ml /c /coff /Zi in.txt.asm
link /OPT:NOREF /DEBUG /SUBSYSTEM:CONSOLE libucrt.lib in.txt.obj 
call in.txt.exe

