//
// Created by 12167 on 2023/7/11.
//

#ifndef C22V1_FIX_ARRAY_H
#define C22V1_FIX_ARRAY_H

#include "travel.h"

int get_dimension(Value* value);
void fix_array(struct _InstNode *instruction_node);
void fix_array2(struct _InstNode *instruction_node);

bool can_cut(Instruction* instruction);        //为前部分提供的化简判断
bool can_cut_after_(Instruction* instruction); //为后部分提供的化简判断

#endif //C22V1_FIX_ARRAY_H
