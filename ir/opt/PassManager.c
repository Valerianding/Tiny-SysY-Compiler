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
    sideEffect(currentFunction);

    DVNT(currentFunction);

    //for loopAnalysis
    loopAnalysis(currentFunction);
    LICM(currentFunction);

    //loop2Memset(currentFunction);

    GCM(currentFunction);

    //instruction_combination(currentFunction);

    postDominanceAnalysis(currentFunction);

    Mark(currentFunction);

    Sweep(currentFunction);

    renameVariables(currentFunction);
}