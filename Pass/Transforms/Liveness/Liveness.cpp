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


using namespace llvm;
using namespace std;

#define DEBUG_TYPE "Liveness"

using namespace llvm;

namespace {   
struct Liveness : public FunctionPass {
    string func_name = "test";
    static char ID;
    Liveness() : FunctionPass(ID) {}

    map<Value*, string> valueNames;
    map<BasicBlock*, set<string>> UEVar;
    map<BasicBlock*, set<string>> VarKill;
    map<BasicBlock*, set<string>> LiveOut;


    int num = 0;
    bool reduntant = 0;

    bool runOnFunction(Function &F) override {
	    
        for (auto& basic_block : F)
        {
            for (auto& inst : basic_block)
            {
                if(inst.getOpcode() == Instruction::Store){
                    // put expression variables into UEVar
                    if (valueNames.find(inst.getOperand(0)) != valueNames.end()) {
                        UEVar[&basic_block].insert(valueNames[inst.getOperand(0)]);
                    }
                    // put the updated variable into VarKill
                    VarKill[&basic_block].insert(inst.getOperand(1)->getName().str());
                }
                if(inst.getOpcode() == Instruction::Load){
                   valueNames[&inst] = inst.getOperand(0)->getName().str();
                }

                if (inst.isBinaryOp() || inst.getOpcode() == 52)
                {
                    // checks operand 1, 2, if it doesnt find in VarKill, 
                    // then we insert the variables to UEVar
                    // string operand1 = valueNames[inst.getOperand(0)];
                    // Value* operand2 = inst.getOperand(1);

                    if (VarKill[&basic_block].find(valueNames[inst.getOperand(0)]) == VarKill[&basic_block].end()){
                        UEVar[&basic_block].insert(valueNames[inst.getOperand(0)]);
                    }

                    if (VarKill[&basic_block].find(valueNames[inst.getOperand(1)]) == VarKill[&basic_block].end()){
                        UEVar[&basic_block].insert(valueNames[inst.getOperand(1)]);
                    }
                } 
            } // end for inst
        } // end for block

        bool cont = true;
        set<string> dest1;
        set<string> dest2;
        set<string> dest3;

        while(cont) {
            cont = false;
            for (auto& basic_block : F) {
                dest3 = LiveOut[&basic_block];
                for (BasicBlock *Succ : successors(&basic_block)) {
                    dest1.clear();
                    dest2.clear();
                    set_difference(LiveOut[Succ].begin(), LiveOut[Succ].end(), 
                                    VarKill[Succ].begin(), VarKill[Succ].end(),
                                    inserter(dest1, dest1.begin()));
                    set_union(dest1.begin(), dest1.end(), 
                                    UEVar[Succ].begin(), UEVar[Succ].end(),
                                    inserter(dest2, dest2.begin()));
                    set_union(dest2.begin(), dest2.end(), 
                                    dest3.begin(), dest3.end(),
                                    inserter(dest3, dest3.begin()));
                }

                if (! (LiveOut[&basic_block] == dest3)) {
                    cont = true;
                }
                LiveOut[&basic_block] = dest3;
            }
        }

        for (auto& basic_block :  F) {
            errs () << "-----" << basic_block.getName() << "------\n";
            errs () << "UEVar: ";
            for (auto it : UEVar[&basic_block]) {
                errs() << it << " ";
            }
            errs () << "\n";
            errs () << "VarKill: ";
            for (auto it : VarKill[&basic_block]) {
                errs () << it << "";
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
