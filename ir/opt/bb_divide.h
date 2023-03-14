//
// Created by Valerian on 2023/1/15.
//

#ifndef C22V1_BB_DIVIDE_H
#define C22V1_BB_DIVIDE_H
#include "bblock.h"
#include "function.h"
#include "stack.h"
// 全局的栈
extern HashMap *GlobalIncomingVal;
HashMap  *GlobalIncomingVal;
void bblock_divide(InstNode *head);
#endif