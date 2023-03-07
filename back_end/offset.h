//
// Created by tom on 23-2-25.
//

#ifndef C22V1_OFFSET_H
#define C22V1_OFFSET_H
//#include "instruction.h"
#include "bblock.h"
//单向链表
typedef struct offset{
    Value *value;
    struct offset*next;
}offset;
offset *new_offset_node();
offset *offset_init(InstNode *ins);
void offset_head_free(offset *offset_head);
#endif //C22V1_OFFSET_H
