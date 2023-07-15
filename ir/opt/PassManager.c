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
    DVNT(currentFunction);
    memlvn(currentFunction);


    //for loop
    loop(currentFunction);
    LICM(currentFunction);


    Mark(currentFunction);
    Sweep(currentFunction);
    renameVariables(currentFunction);
}