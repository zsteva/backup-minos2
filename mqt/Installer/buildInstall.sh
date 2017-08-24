#! /bin/bash

cd ~

if [ -d ./minos_runtime ]; then
  rm -rf ./minos_runtime
fi

mkdir ./minos_runtime

cd ./minos_runtime

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
cp ~/Minos2/build-mqt-Desktop-Release/MqtServer/MqtServer Bin

cp ~/Minos2/mqt/ControlFiles/Configuration/* ./Configuration

cd Configuration

wget http://www.country-files.com/cty/cty.dat -O cty.dat
wget http://www.rsgbcc.org/vhf/vhfcontests16.xml -O vhfcontests16.xml
wget http://www.rsgbcc.org/vhf/vhfcontests17.xml -O vhfcontests17.xml

