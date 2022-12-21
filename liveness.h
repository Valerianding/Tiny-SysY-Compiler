//
// Created by Valerian on 2022/12/2.
//

#ifndef C22V1_LIVENESS_H
#define C22V1_LIVENESS_H

#include "instruction.h"
#include "bblock.h"
//typedef struct _livenessnode{
//    Value *this;
//    struct _livenessnode *prev;
//    struct _livenessnode *next;
//}livenessnode;
//
//livenessnode *ll_create(Value *this);
//
//void ll_add_tail(livenessnode *head,Value *this);
//
//int ll_isexsit(livenessnode *head,Value *this);



void ll_analysis(BasicBlock *this);

void copy_liveness(InstNode *this,InstNode *next);

void printliveness(BasicBlock *this);
#endif //C22V1_LIVENESS_H
