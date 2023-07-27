//
// Created by Valerian on 2023/7/27.
//

#ifndef C22V1_SCCP_H
#define C22V1_SCCP_H
#include "utility.h"
#include "function.h"
typedef enum LatticeType{
    NotConstant, //not a constant -> bottom
    Constant,
    MayBeConstant, // may be constant -> top
}LatticeType;

typedef struct LatticeValue{
    LatticeType type;
    Value *val;
}LatticeValue;

extern List *CFGWorkList; //BasicBlock *
extern List *SSAWorkList; //Instruction *
extern HashSet *ExecutedMap; // executed Blocks
extern HashMap *LatticeMap; // Value * -> LatticeValue;
void EvaluateAssignAndTerminator(InstNode *instNode);
void SCCP(Function *currentFunction);

#endif //C22V1_SCCP_H
