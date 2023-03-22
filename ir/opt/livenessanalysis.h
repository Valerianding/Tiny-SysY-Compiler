//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_LIVENESSANALYSIS_H
#define C22V1_LIVENESSANALYSIS_H
#include "function.h"
#include "utility.h"
void calculateLiveness(Function *currentFunction);
bool HashSetCopyValue(HashSet *dest, HashSet *src);
bool analysisBlock(BasicBlock *block, HashSet *nextLiveIn);
void printLiveness(BasicBlock *block);
#endif //C22V1_LIVENESSANALYSIS_H
