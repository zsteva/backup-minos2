#!/bin/bash


brew update -y
brew upgrade -y

# brew install git -y
# brew install git-gui -y

# brew install cmake -y

# brew install build-essential 		


# qt5-default qtcreator -y
# brew install qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins libqt5multimedia5 -y
# brew install libqt5serialport5 -y
# brew install libqt5serialport5-dev -y

# brew install libhamlib2 -y
# brew install libhamlib-dev -y

# brew install libasound2 -y
# brew install libasound2-dev -y




brew install git 
# brew install git-gui -y
brew install wget
brew install cmake qt5 autoconf hamlib # /usr/local/Cellar/hamlib/1.2.15.3

brew cask install qt-creator


# brew install build-essential 		- Looks like xCode should provide most of this. 


qt5-default qtcreator -y
brew install qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins libqt5multimedia5 -y
brew install libqt5serialport5 -y
brew install libqt5serialport5-dev -y

brew install libhamlib2 -y
brew install libhamlib-dev -y

brew install libasound2 -y
brew install libasound2-dev -y




brew autoremove -y


