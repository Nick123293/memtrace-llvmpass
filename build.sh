#!/bin/bash
rm -rf build
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=/home/and21829/llvm/llvm_19 \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang    \
    ..
make
cd ../
clang++ -S -emit-llvm seq_add.cpp -o seq_add.ll