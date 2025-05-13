#opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace -disable-output test-file.ll
opt -load-pass-plugin ./build/libtracemem.so -passes=memory-trace test-file.ll -S -o modified-test.ll