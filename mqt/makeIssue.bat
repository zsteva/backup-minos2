cd ..\QTbuild

rmdir /Q /S Issue

mkdir Issue
cd Issue

copy ..\AdjQtTop\AdjQt\release\AdjQt.exe

copy d:\Qt\5.5\msvc2013\bin\Qt5Widgets.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Core.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Gui.dll
copy d:\Qt\5.5\msvc2013\bin\Qt5Network.dll

mkdir platforms
copy d:\Qt\5.5\msvc2013\plugins\platforms\qwindows.dll platforms

copy C:\Windows\System32\msvcp120.dll
copy C:\Windows\System32\msvcrt.dll
copy C:\Windows\System32\msvcr120.dll

xcopy /E /F /Y ..\..\AdjQtRoot\AdjBaseEnv

del /Q database\*.*

REM download latest versions of files

cd control
\bin\wget\wget http://www.country-files.com/cty/cty.dat -O cty.dat

cd ..

\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests13.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/hfcontests13.xml
\bin\wget\wget http://bartg.rsgbcc.org/bartgcontests13.xml
\bin\wget\wget http://microwave.rsgbcc.org/microcontests13.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests14.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/hfcontests14.xml
\bin\wget\wget http://bartg.rsgbcc.org/bartgcontests14.xml
\bin\wget\wget http://microwave.rsgbcc.org/microcontests14.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests15.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/hfcontests15.xml
\bin\wget\wget http://bartg.rsgbcc.org/bartgcontests15.xml
\bin\wget\wget http://microwave.rsgbcc.org/microcontests15.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests16.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/hfcontests16.xml
\bin\wget\wget http://bartg.rsgbcc.org/bartgcontests16.xml
\bin\wget\wget http://microwave.rsgbcc.org/microcontests16.xml
\bin\wget\wget http://www.rsgbcc.org/cgi-bin/vhfenter.pl?afsdownload=y -O clublist.txt


cd ..\..\AdjQtRoot

