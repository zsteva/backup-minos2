#!/bin/bash


apt-get update -y
apt-get upgrade -y
apt-get install cmake -y
apt-get install build-essential qt5-default qtcreator -y
apt-get install qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins libqt5multimedia5 -y
apt-get install libqt5serialport5 -y
apt-get install libqt5serialport5-dev -y
apt-get install git -y
apt-get install git-gui -y


apt-get autoremove -y

