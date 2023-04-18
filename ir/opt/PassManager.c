//
// Created by Valerian on 2023/4/15.
//

#include "PassManager.h"
void RunPasses(Function *currentFunction){
    bool effective = false;
    effective |= commonSubexpressionElimination(currentFunction);
    effective |= ConstFolding(currentFunction);
    if(effective) RunPasses(currentFunction);
}