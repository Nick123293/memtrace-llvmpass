#!/bin/bash
#one line compilation of pass
clang++ $(llvm-config --cxxflags) -fPIC -shared ../tracemem.cpp -o MemoryTrace.so
