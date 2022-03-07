cd ../../Pass/build/
cmake -DCMAKE_BUILD_TYPE=Release ../Transforms/Liveness
make -j4
cd ../../test/Liveness/
clang -S -emit-llvm test$1.c -o test$1.ll
opt -load ../../Pass/build/libLLVMLivenessPass.so -enable-new-pm=0 -Liveness test$1.ll
