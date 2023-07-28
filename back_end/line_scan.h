//
// Created by Administrator on 2023/7/27.
//

#ifndef C22V1_LINE_SCAN_H
#define C22V1_LINE_SCAN_H
#include "live_interval.h"
typedef struct _value_register{
    int reg;
}value_register;

PriorityQueue *get_function_live_interval(InstNode*ins);
void line_scan(InstNode*ins);
void line_scan_alloca(PriorityQueue*queue);
int CompareNumerics2(const void* lhs, const void* rhs);
int get_an_availabel_register();
void free_register(int i);
void expire_old_intervals(value_live_range *i);
void spill_at_interval(value_live_range *i);
value_live_range* get_last_interval_in_active();
void pop_last_interval_in_active();
#endif //C22V1_LINE_SCAN_H
