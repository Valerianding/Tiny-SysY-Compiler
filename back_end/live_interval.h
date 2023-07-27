//
// Created by ljf on 2023/7/26.
//

#ifndef C22V1_LIVE_INTERVAL_H
#define C22V1_LIVE_INTERVAL_H

#include "bblock.h"
#include "function.h"
#include "instruction.h"
#include "symtab.h"
#include "type.h"
#include "use.h"
#include "value.h"
#include "container/priority_queue.h"
#include "container/hash_map.h"
#include "container/vector.h"
#include "container/hash_set.h"
#include "ir/opt/utility.h"

typedef struct _live_range{
    int start;
    int end;
}live_range;
typedef struct _value_live_range{
    Value *value;
    int start;
    int end;
}value_live_range;

int CompareNumerics(const void* lhs, const void* rhs);
void init_PriorityQueue(PriorityQueue*queue);
void init_HashMap(HashMap*hashMap);
PriorityQueue *build_live_interval(Vector* vector);
void analyze_block();
void analyze_ins(InstNode *ins);
void handle_def(Value*dvalue,int ins_id);
void handle_use(Value*uvalue,int ins_id);
bool value_is_in_liveout(Value*tvalue);
void print_live_interval();
#endif //C22V1_LIVE_INTERVAL_H
