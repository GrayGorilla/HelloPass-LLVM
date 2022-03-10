#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include <string>
#include <map>
#include <set>
#include "llvm/IR/CFG.h"
#include "llvm/IR/BasicBlock.h"

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "Liveness"

namespace {   
struct Liveness : public FunctionPass {
    string func_name = "test";
    static char ID;
    Liveness() : FunctionPass(ID) {}

    map<Value*, string> varNames;
    map<BasicBlock*, set<string>> UEVar;
    map<BasicBlock*, set<string>> VarKill;
    map<BasicBlock*, set<string>> LiveOut;

    bool runOnFunction(Function &F) override {
        errs() << "LivenessAnalysisPass: ";	    
        for (auto& basic_block : F) {
            for (auto& inst : basic_block) {   
                if(inst.getOpcode() == Instruction::Store) {
                    // put expression variables into UEVar
                    if (varNames.find(inst.getOperand(0)) != varNames.end()) {
                        UEVar[&basic_block].insert(varNames[inst.getOperand(0)]);
                    }
                    // put the updated variable into VarKill                    
                    VarKill[&basic_block].insert(inst.getOperand(1)->getName().str());
                }

                if(inst.getOpcode() == Instruction::Load) {
                    varNames[&inst] = inst.getOperand(0)->getName().str();
                }

                if (inst.isBinaryOp() || inst.getOpcode() == 52 || inst.getOpcode() == 53) {
                    if (VarKill[&basic_block].find(varNames[inst.getOperand(0)]) == VarKill[&basic_block].end()) {
                        UEVar[&basic_block].insert(varNames[inst.getOperand(0)]);
                    }

                    if (VarKill[&basic_block].find(varNames[inst.getOperand(1)]) == VarKill[&basic_block].end()) {
                        UEVar[&basic_block].insert(varNames[inst.getOperand(1)]);
                    }
                } 
            } // end for inst
        }

        bool cont = true;
        set<string> set1;
        set<string> set2;
        set<string> set3;

        while(cont) {
            cont = false;
            for (auto& basic_block : F) {
                set3 = LiveOut[&basic_block];
                for (BasicBlock *Succ : successors(&basic_block)) {
                    set1.clear();
                    set2.clear();
                    set_difference(LiveOut[Succ].begin(), LiveOut[Succ].end(), 
                                    VarKill[Succ].begin(), VarKill[Succ].end(),
                                    inserter(set1, set1.begin()));
                    set_union(set1.begin(), set1.end(), 
                            UEVar[Succ].begin(), UEVar[Succ].end(),
                            inserter(set2, set2.begin()));
                    set_union(set2.begin(), set2.end(), 
                            set3.begin(), set3.end(),
                            inserter(set3, set3.begin()));
                }

                if (! (LiveOut[&basic_block] == set3)) {
                    cont = true;
                }
                LiveOut[&basic_block] = set3;
            }
        }

        for (auto& basic_block :  F) {
            // errs () << "-----" << basic_block.getName() << "------\n";
            errs () << "UEVar: ";
            for (auto it : UEVar[&basic_block]) {
                errs() << it << " ";
            }
            errs () << "\n";
            errs () << "VarKill: ";
            for (auto it : VarKill[&basic_block]) {
                errs () << it << " ";
            }
            errs () << "\n";
            errs () << "LiveOut: ";
            for (auto it : LiveOut[&basic_block]) {
                errs () << it << " ";
            }
            errs () << "\n";
        }
        return false;
    } // end runOnFunction
}; // end of struct 
}  // end of anonymous namespace

char Liveness::ID = 0;
static RegisterPass<Liveness> X("Liveness", "Liveness Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
