cd ../../Pass/build/
cmake -DCMAKE_BUILD_TYPE=Release ../Transforms/Liveness
make -j4
cd ../../test/Liveness/
clang -S -emit-llvm -fno-discard-value-names test4.c -o test4.ll
opt -load ../../Pass/build/libLLVMLivenessPass.so -enable-new-pm=0 -Liveness test4.ll
