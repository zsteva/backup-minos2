cd ..\QTbuild

rmdir /Q /S Issue

mkdir Issue
cd Issue

copy ..\msvc\mqt\MqtLogger\release\MqtLogger.exe

copy d:\Qt\5.5\msvc2013\bin\Qt5Widgets.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Core.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Gui.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Network.dll

mkdir platforms
copy d:\Qt\5.5\msvc2013\plugins\platforms\qwindows.dll platforms

copy C:\Windows\System32\msvcp120.dll
copy C:\Windows\System32\msvcrt.dll
copy C:\Windows\System32\msvcr120.dll

xcopy /E /F /Y ..\..\mqt\MqtBaseEnv


REM download latest versions of files

cd configuration
\bin\wget\wget http://www.country-files.com/cty/cty.dat -O cty.dat

cd ..

\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests16.xml


cd ..\..\sfMinos

