//
// Created by 12167 on 2023-05-10.
//

#ifndef C22V1_INLINER_H
#define C22V1_INLINER_H

#include "instruction.h"
#include "bblock.h"
#include "value_stack.h"
#include "past_stack.h"
#include "insnode_stack.h"
#include "symtab.h"
#include <stdio.h>
#include <stdbool.h>
#include "mem2reg.h"
#include "travel.h"

void func_inline(struct _InstNode* instruction_node);
InstNode *find_func_begin(struct _InstNode* instruction_node,char* func_name);
void renameV(Function *currentFunction);
void label_func_inline(struct _InstNode* instNode_list);

#endif //C22V1_INLINER_H
