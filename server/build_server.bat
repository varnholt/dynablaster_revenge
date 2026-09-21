call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\6.10.1\msvc2022_64\bin;%PATH%
cd /d D:\git\dynablaster_revenge\server
qmake server.pro -o Makefile
nmake /f Makefile
