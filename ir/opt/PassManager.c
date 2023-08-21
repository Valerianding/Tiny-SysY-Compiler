//
// Created by Valerian on 2023/4/15.
//

#include "PassManager.h"
void RunBasicPasses(Function *currentFunction){
    bool effective = true;

    while(effective){
        effective = false;
        ConstFolding(currentFunction);
        commonSubexpressionElimination(currentFunction);
    }

    renameVariables(currentFunction);

}

void RunOptimizePasses(Function *currentFunction){

    BranchOptimizing(currentFunction);

    if(hasSingleExit(currentFunction)){
        postDominanceAnalysis(currentFunction);

        Mark(currentFunction);

        Sweep(currentFunction);

        removeUnreachable(currentFunction);
    }

    dominanceAnalysis(currentFunction);

    DVNT(currentFunction); // 浮点情况还有可能有错

    //TODO BUG & NOT Simplified
    //memlvn(currentFunction);

    //for loopAnalysis
    loopAnalysis(currentFunction);

    LICM(currentFunction);
//
    Loop2Memset(currentFunction);
//
    Loop2Memcpy(currentFunction);

    memOpt(currentFunction);

    GCM(currentFunction);

    instruction_combination(currentFunction);

    bool changed = true;
    while(changed){
        changed =  InstCombine(currentFunction);
        renameVariables(currentFunction);
    }

    if(hasSingleExit(currentFunction)){
        postDominanceAnalysis(currentFunction);

        Mark(currentFunction);

        Sweep(currentFunction);

        removeUnreachable(currentFunction);
    }

    Clean(currentFunction);

    renameVariables(currentFunction);
}