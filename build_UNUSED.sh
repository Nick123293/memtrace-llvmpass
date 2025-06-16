#!/bin/bash
rm -rf build
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=/home/and21829/llvm/llvm_19/build \
    -DCMAKE_CXX_COMPILER=`which clang++` \
    -DCMAKE_C_COMPILER=`which clang`    \
    ..
make
cd ../