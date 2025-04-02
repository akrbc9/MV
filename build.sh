#!/bin/zsh

rm -rf build
mkdir build && cd build 
cmake ..
make
make setup_python_api