#! /bin/bash

cd ~/Minos2

git pull origin master

if [! -d ./build ]; then
   mkdir build
fi

cd build

qmake.exe ..\mqt\mqt.pro

make release



if [ -d ./minos-runtime ]; then
  rm -rf ./minos-runtime
fi

mkdir ./minos-runtime

cd ./minos-runtime

mkdir Configuration
mkdir Lists
mkdir Logs
mkdir Bin

cp ~/Minos2/build-mqt-Desktop-Release/MqtAppStarter/MqtAppStarter Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtChat/MqtChat Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtControl/MqtControl Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtKeyer/MqtKeyer Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtLogger/MqtLogger Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtMonitor/MqtMonitor Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtRigControl/MqtRigControl Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtRotator/MqtRotator Bin
cp ~/Minos2/build-mqt-Desktop-Release/MqtServer/MqtServer Bin

cp ~/Minos2/mqt/ControlFiles/Configuration/* ./Configuration
cp ~/Minos2/mqt/ControlFiles/Configuration/LinuxFiles/* ./Configuration
cp ~/Minos2/mqt/Installer/Minos.sh . 

cd Configuration

wget http://www.country-files.com/cty/cty.dat -O cty.dat
wget http://www.rsgbcc.org/vhf/vhfcontests16.xml -O vhfcontests16.xml
wget http://www.rsgbcc.org/vhf/vhfcontests17.xml -O vhfcontests17.xml

