cd ../../Pass/build/
cmake -DCMAKE_BUILD_TYPE=Release ../Transforms/ValueNumbering
make -j4
cd ../../test/ValueNumbering/
clang -S -emit-llvm test$1.c -o test$1.ll
opt -load ../../Pass/build/libLLVMValueNumberingPass.so -enable-new-pm=0 -ValueNumbering test$1.ll
