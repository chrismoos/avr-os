#!/usr/bin/env bash
set -e

sudo apt-get -q -y install gcc-avr avr-libc libelf-dev pkg-config freeglut3 freeglut3-dev
mkdir -p build
cd build
git clone git://gitorious.org/simavr/simavr
cd simavr && sudo make install
cd ../
wget -O "arduino.tar.gz" "http://arduino.googlecode.com/files/arduino-1.0.3-linux64.tgz"
tar xzf arduino.tar.gz
export AVR_HOME="$(pwd)/arduino-1.0.3"
