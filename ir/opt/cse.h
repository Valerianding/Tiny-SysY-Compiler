//
// Created by Valerian on 2023/4/16.
//

#ifndef C22V1_CSE_H
#define C22V1_CSE_H
#include "function.h"
#include "utility.h"
bool commonSubexpressionElimination(Function *currentFunction);
bool commonSubexpression(BasicBlock *block,Function *currentFunction);
#endif //C22V1_CSE_H
