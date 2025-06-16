#opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace -disable-output test-file.ll
clang++ -S -emit-llvm seq_add.cpp -o seq_add.ll
opt -load-pass-plugin ./MemoryTrace.so -passes=memory-trace seq_add.ll -S -o modified-seq-add.ll
clang++ modified-seq-add.ll -o modified-seq-add
./modified-seq-add
#cat memory-traces.log
