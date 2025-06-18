#opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace -disable-output test-file.ll
clang++ -S -emit-llvm ../tests/seq_add.cpp -o ../tests/seq_add.ll
opt -load-pass-plugin ../MemoryTrace.so -passes=memory-trace ../tests/seq_add.ll -S -o ../tests/modified-seq-add.ll
clang++ ../tests/modified-seq-add.ll -o ../tests/modified-seq-add
../tests/modified-seq-add
#cat memory-traces.log
