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
mkdir Bin

copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtAppStarter\release\MqtAppStarter.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtChat\release\MqtChat.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtControl\release\MqtControl.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtKeyer\release\MqtKeyer.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtLogger\release\MqtLogger.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtMonitor\release\MqtMonitor.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtRigControl\release\MqtRigControl.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtRotator\release\MqtRotator.exe Bin
copy C:\Projects\sfMinos\QTbuild\gcc\592\mqt\MqtServer\release\MqtServer.exe Bin

copy C:\Projects\hamlib-w32-3.1\bin\*.dll Bin

xcopy /E /F /Y C:\Projects\sfMinos\mqt\ControlFiles\Configuration .\Configuration

cd Configuration

\bin\wget\wget http://www.country-files.com/cty/cty.dat -O cty.dat
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests16.xml -O vhfcontests16.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests17.xml -O vhfcontests17.xml

cd ../Bin

C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtAppStarter.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtChat.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtControl.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtKeyer.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtLogger.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtMonitor.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtRigControl.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtRotator.exe
C:\Qt\5.9.2\mingw53_32\bin\windeployqt.exe MqtServer.exe

cd ../..
mkdir Installer

xcopy /E /F /Y C:\Projects\sfMinos\mqt\Installer .\Installer

C:\"Program Files (x86)\Inno Setup 5\ISCC.exe" Installer\Minos2Install.iss


cd C:\Projects\sfMinos\mqt\Installer