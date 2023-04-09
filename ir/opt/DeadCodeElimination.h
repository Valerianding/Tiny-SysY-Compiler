//
// Created by Valerian on 2023/3/31.
//

#ifndef C22V1_DEADCODEELIMINATION_H
#define C22V1_DEADCODEELIMINATION_H
#include "function.h"

void DeadCodeElimination(Function *currentFunction);
void DeleteUselessBasicBlocks(Function *currentFunction);
#endif //C22V1_DEADCODEELIMINATION_H
