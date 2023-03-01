//
// Created by Valerian on 2023/1/15.
//

#ifndef C22V1_MEM2REG_H
#define C22V1_MEM2REG_H
#include "bblock.h"
#include "function.h"
void mem2reg(Function *currentFunction);
void insert_phi(BasicBlock *block,Value *val);
void updateReachingDef();
#endif //C22V1_MEM2REG_H
