//
// Created by Valerian on 2023/3/31.
//

#ifndef C22V1_DCE_H
#define C22V1_DCE_H
#include "function.h"
#include "utility.h"
#include "dominance.h"
void depthFirstTraversal(Vector *vector, BasicBlock *block);
bool isEssentialOperator(InstNode *inst);
void Mark(Function *currentFunction);
bool Sweep(Function *currentFunction);
void Clean(Function *currentFunction);
bool OnePass(Vector *vector);
void removeBlock(BasicBlock *block);
#endif //C22V1_DCE_H
