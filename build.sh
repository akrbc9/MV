#!/bin/zsh

rm -rf build
mkdir build && cd build 
cmake -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ ..
make
cd bin
./predator_prey

