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
    map<basic_block, set<string>> UEVar;
    map<basic_block, set<string>> VarKill;
    map<basic_block, set<string>> LiveOut;


    int num = 0;
    bool reduntant = 0;

    bool runOnFunction(Function &F) override {
	    
        // errs() << "ValueNumbering: " << F.getName() << "\n";
        // if (F.getName() != func_name) return false;

        for (auto& basic_block : F)
        {
            for (auto& inst : basic_block)
            {
                if(inst.getOpcode() == Instruction::Store){
                    // put expression variables into UEVar
                    if (valueName.find(inst.getOperand(0)) != valueName.end()) {
                        UEVar[&basic_block].insert(valueNames[inst.getOperand(0)]);
                    }
                    // put the updated variable into VarKill
                    VarKill[&basic_block].insert(inst.getOperand(1)->getName().str);
                }
                if(inst.getOpcode() == Instruction::Load){
                   ValueNames[&inst] = inst.getOperand(0) -> getName().str;
                }

                if (inst.isBinaryOp() || inst.getOpcode == 52)
                {
                    // checks operand 1, 2, if it doesnt find in VarKill, 
                    // then we insert the variables to UEVar
                    int operand1 = valueNames[inst.getOperand(0)];
                    int operand2 = inst.getOperand(1);

                    if VarKill[&basic_block].find(operand1) == VarKill[&basic_block].end()){
                        UEVar[&basic_block].insert(operand1);
                    }

                    if VarKill[&basic_block].find(operand2) == VarKill[&basic_block].end()){
                        UEVar[&basic_block].insert(operand2);
                    }
// --------------------------------------------------------------------------------------

                    // Let's check if "operand1 opcode operand2" already exists
                    string opcode =  inst.getOpcodeName();
                    string instruction = to_string(operand1) + " " + opcode + " " + to_string(operand2);

                    if (oprtnsNum.find(instruction) == oprtnsNum.end()){
                        // Not exist. Let's give it LVN
                        oprtnsNum[instruction] = ++num;        
                        reduntant = 0;
                    }
                    else reduntant = 1;
                    // Make sure we give same LVN to destination 
                    oprndsNum[&inst] = oprtnsNum[instruction];

                    errs() << inst << "\t\t" << oprndsNum[&inst] << " = " << instruction;
                    if (reduntant) errs() << "\t (redundant)";
                    errs() << "\n";
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
