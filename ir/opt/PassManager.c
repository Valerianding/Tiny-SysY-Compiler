//
// Created by Valerian on 2023/4/15.
//

#include "PassManager.h"
void RunPasses(Function *currentFunction){
    bool effective = false;

    effective |= ConstFolding(currentFunction);
    effective |= commonSubexpressionElimination(currentFunction);

    //DCE
    Mark(currentFunction);
    effective |= Sweep(currentFunction);


    calculateLiveness(currentFunction);
    loop(currentFunction);
    //CFG clean
    Clean(currentFunction);
    if(effective) RunPasses(currentFunction);
}