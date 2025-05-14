#opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace -disable-output test-file.ll
opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace seq_add.ll -S -o modified-seq-add.ll
clang++ modified-seq-add.ll -o modified-seq-add
./modified-seq-add
cat memory-traces.log