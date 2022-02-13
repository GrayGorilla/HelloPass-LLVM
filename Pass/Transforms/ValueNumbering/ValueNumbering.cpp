#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include <string>
#include <unordered_map>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "ValueNumbering"

using namespace llvm;

namespace {
struct ValueNumbering : public FunctionPass {
    string func_name = "test";
    static char ID;
    int valCounter;
    unordered_map<Value*, int> registers;
    unordered_map<string, int> binaryExpr;

    ValueNumbering() : FunctionPass(ID), valCounter(0) {}

    int processSrc(Value* reg) {
        auto itr = registers.find(reg);
       if (itr == registers.end()) {
            valCounter++;
            registers.insert({reg, valCounter});
            return valCounter;
        } else {
            return itr->second;
        }
    }

    void processDest(Value* reg, int valueID) {
        auto itr = registers.find(reg);
        if (itr == registers.end()) {
            registers.insert({reg, valueID});
        } else {
            itr->second = valueID;
        }
    }

    int processBinOp(int a, int b, char sign) {
        string keyA = to_string(a) + sign + to_string(b);
        string keyB = to_string(b) + sign + to_string(a);
        auto itr = binaryExpr.find(keyA);

        if (itr == binaryExpr.end()) {
            itr = binaryExpr.find(keyB);
            if (itr == binaryExpr.end()) {
                valCounter++;
                binaryExpr.insert({keyA, valCounter});
                return valCounter;
            }
        } 
        return itr->second;
    }


    bool runOnFunction(Function &F) override {
	    
        errs() << "ValueNumbering: " << F.getName() << "\n";
        if (F.getName() != func_name) return false;

        for (auto& basic_block : F)
        {
            for (auto& inst : basic_block)
            {
                errs() << inst << "\n";
                if(inst.getOpcode() == Instruction::Load){
                    Value* srcReg = inst.getOperand(0);
                    Value* destReg = &inst;
                    int srcID = processSrc(srcReg);
                    processDest(destReg, srcID);

                    errs() << "\t\t\t" << srcID << " = " << srcID << "\n";
                    errs() << "This is Load"<<"\n";
                    errs() << "&inst: " << &inst << "\n";
                    errs() << "operand 0: " << inst.getOperand(0) << "\n";
                    if (inst.isBinaryOp()) {
                        errs() << "BINARY!! \n";
                    } else {
                        errs() << "NOT binary..\n";
                    }
                }
                if(inst.getOpcode() == Instruction::Store){
                    Value* srcReg = inst.getOperand(0);
                    Value* destReg = inst.getOperand(1);
                    int srcID = processSrc(srcReg);
                    processDest(destReg, srcID);

                    errs() << "\t\t\t" << srcID << " = " << srcID << "\n";
                    errs() << "This is Store"<<"\n";
                    errs() << "operand 0: " << inst.getOperand(0) << "\n";
                    errs() << "operand 1: " << inst.getOperand(1) << "\n";
                    if (inst.isBinaryOp()) {
                        errs() << "BINARY!! \n";
                    } else {
                        errs() << "NOT binary..\n";
                    }

                }
                if (inst.isBinaryOp())
                {
                    errs() << "Op Code:" << inst.getOpcodeName()<<"\n";
                    if(inst.getOpcode() == Instruction::Add){
                        Value* srcRegA = inst.getOperand(0);
                        Value* srcRegB = inst.getOperand(1);
                        Value* destReg = &inst;
                        int srcIDA = processSrc(srcRegA);
                        int srcIDB = processSrc(srcRegB);
                        int binID = processBinOp(srcIDA, srcIDB, '+');
                        processDest(destReg, binID);

                        errs() << "\t\t\t" << binID << " = " << srcIDA << " add " << srcIDB << "\n";
                        errs() << "This is Addition"<<"\n";
                        errs() << "&inst: " << &inst << "\n";
                        errs() << "operand 0: " << inst.getOperand(0) << "\n";
                        errs() << "operand 1: " << inst.getOperand(1) << "\n";
                        if (inst.isBinaryOp()) {
                            errs() << "BINARY!! \n";
                        } else {
                            errs() << "NOT binary..\n";
                        }
                    }
                    if(inst.getOpcode() == Instruction::Mul){
                        Value* srcRegA = inst.getOperand(0);
                        Value* srcRegB = inst.getOperand(1);
                        Value* destReg = &inst;
                        int srcIDA = processSrc(srcRegA);
                        int srcIDB = processSrc(srcRegB);
                        int binID = processBinOp(srcIDA, srcIDB, '*');
                        processDest(destReg, binID);
                        
                        errs() << "\t\t\t" << binID << " = " << srcIDA << " mult " << srcIDB << "\n";
                        errs() << "This is Multiplication"<<"\n";
                        errs() << "&inst: " << &inst << "\n";
                        errs() << "operand 0: " << inst.getOperand(0) << "\n";
                        errs() << "operand 1: " << inst.getOperand(1) << "\n";
                        if (inst.isBinaryOp()) {
                            errs() << "BINARY!! \n";
                        } else {
                            errs() << "NOT binary..\n";
                        }
                    }
                    
                    // see other classes, Instruction::Sub, Instruction::UDiv, Instruction::SDiv
                    // errs() << "Operand(0)" << (*inst.getOperand(0))<<"\n";
                    auto* ptr = dyn_cast<User>(&inst);
		    //errs() << "\t" << *ptr << "\n";
                    for (auto it = ptr->op_begin(); it != ptr->op_end(); ++it) {
                        errs() << "\t" <<  *(*it) << "\n";
                        // if ((*it)->hasName()) 
			    // errs() << (*it)->getName() << "\n";                      
                    }
                } // end if
            } // end for inst
        } // end for block
        return false;
    } // end runOnFunction
}; // end of struct ValueNumbering
}  // end of anonymous namespace

char ValueNumbering::ID = 0;
static RegisterPass<ValueNumbering> X("ValueNumbering", "ValueNumbering Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
