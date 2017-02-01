#!/usr/bin/env bash
set -e

sudo apt-get -q -y install gcc-avr avr-libc libelf-dev pkg-config freeglut3 freeglut3-dev
mkdir -p build
cd build
git clone https://github.com/buserror/simavr
cd simavr && sudo make install
cd ../
wget -O "arduino.tar.gz" "http://downloads.arduino.cc/arduino-1.0.3-linux64.tgz"
tar xzf arduino.tar.gz
