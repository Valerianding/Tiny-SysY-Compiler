//
// Created by Valerian on 2023/1/15.
//

#ifndef C22V1_MEM2REG_H
#define C22V1_MEM2REG_H
#include "bblock.h"
/*
 * 1.def : store user : load
 * 移除没有user的alloca
 * 如果只有一个def 那么所有user都可以换成store
 * 保证 1.在一个基本块 或者 能够被支配
 *
 *
 */
void mem2reg(InstNode *head);

void delete_useless_store(InstNode *head,Value *value);
#endif //C22V1_MEM2REG_H
