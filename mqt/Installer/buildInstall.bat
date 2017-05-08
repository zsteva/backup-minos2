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

copy C:\Projects\sfMinos\QTbuild\gcc\mqt\MqtLogger\release\MqtLogger.exe

xcopy /E /F /Y C:\Projects\sfMinos\mqt\ControlFiles\Configuration .\Configuration

cd Configuration

\bin\wget\wget http://www.country-files.com/cty/cty.dat -O cty.dat
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests16.xml
\bin\wget\wget http://www.rsgbcc.org/vhf/vhfcontests17.xml

cd ..

C:\Qt\5.6\mingw49_32\bin\windeployqt.exe MqtLogger.exe

REM \bin\7za a ..\MqtLoggerFiles.7z *

cd ..
mkdir Installer

xcopy /E /F /Y C:\Projects\sfMinos\mqt\Installer .\Installer
xcopy /E /F /Y .\installFiles .\Installer\packages\Logger\data

C:\Qt\Tools\QtInstallerFramework\2.0\bin\binarycreator.exe --offline-only -t C:\Qt\Tools\QtInstallerFramework\2.0\bin\installerbase.exe -p C:\temp\mqtInstaller\Installer\packages -c C:\temp\mqtInstaller\Installer\config\config.xml Minos2Install_0_3_0.exe


cd C:\Projects\sfMinos\mqt\Installer