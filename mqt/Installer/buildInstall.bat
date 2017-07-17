C:
if not exist \temp mkdir \temp

cd \temp
if exist mqtInstaller rmdir /Q /S mqtInstaller
mkdir mqtInstaller
cd mqtInstaller
mkdir installFiles
cd installFiles

mkdir Configuration
mkdir Lists
mkdir Logs

copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtAppStarter\release\MqtAppStarter.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtChat\release\MqtChat.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtControl\release\MqtControl.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtKeyer\release\MqtKeyer.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtLogger\release\MqtLogger.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtRotator\release\MqtRotator.exe
copy C:\Projects\sfMinos\QTbuild\gcc\562\mqt\MqtServer\release\MqtServer.exe

xcopy /E /F /Y C:\Projects\sfMinos\mqt\ControlFiles\Configuration .\Configuration

cd Configuration

\bin\wget\wget http://www.country-files.com/cty/cty.dat -O cty.dat
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests16.xml -O vhfcontests16.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests17.xml -O vhfcontests17.xml

cd ..

C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtAppStarter.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtChat.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtControl.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtKeyer.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtLogger.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtRotator.exe
C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtServer.exe

cd ..
mkdir Installer

xcopy /E /F /Y C:\Projects\sfMinos\mqt\Installer .\Installer

C:\"Program Files (x86)\Inno Setup 5\ISCC.exe" Installer\Minos2Install.iss


cd C:\Projects\sfMinos\mqt\Installer