//
// Created by Valerian on 2023/4/15.
//

#include "PassManager.h"
void RunBasicPasses(Function *currentFunction){
    bool effective = true;

    while(effective){
        effective = false;
        effective |= ConstFolding(currentFunction);
        effective |=  commonSubexpressionElimination(currentFunction);
    }

    renameVariables(currentFunction);

}

void RunOptimizePasses(Function *currentFunction){

    dominanceAnalysis(currentFunction);

    DVNT(currentFunction); // 浮点情况还有可能有错

    //for loopAnalysis
    loopAnalysis(currentFunction);
//
    LICM(currentFunction);

    Loop2Memset(currentFunction);

    Loop2Memcpy(currentFunction);

    GCM(currentFunction);

    instruction_combination(currentFunction);

    postDominanceAnalysis(currentFunction);

    Mark(currentFunction);

    Sweep(currentFunction);

    removeUnreachable(currentFunction);

    renameVariables(currentFunction);
}