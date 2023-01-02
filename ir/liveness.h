//
// Created by Valerian on 2022/12/2.
//

#ifndef C22V1_LIVENESS_H
#define C22V1_LIVENESS_H

#include "instruction.h"
#include "bblock.h"
#include "hash_map.h"

void ll_analysis(BasicBlock *this);

void copy_liveness(InstNode *this,InstNode *next);

void printliveness(BasicBlock *this);
#endif //C22V1_LIVENESS_H
