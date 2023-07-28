//
// Created by Administrator on 2023/7/27.
//

#ifndef C22V1_LINE_SCAN_H
#define C22V1_LINE_SCAN_H
#include "live_interval.h"
typedef struct _value_register{
    int reg;
}value_register;

void get_function_live_interval(Function*curFunction);
void line_scan(InstNode*ins,Function* start);
void line_scan_alloca(Function *curFunction,PriorityQueue*queue);

int CompareNumerics(const void* lhs, const void* rhs);
int CompareNumerics2(const void* lhs, const void* rhs);
int get_an_availabel_register();
void free_register(int i);

void expire_old_intervals(Function *curFunction,value_live_range *i);
void spill_at_interval(Function *curFunction,value_live_range *i);

//当变量的end是一样的时候，二者操作的不一定是同一个value，所以会出错
value_live_range* get_last_interval_in_active();
void pop_last_interval_in_active(value_live_range*spill);


void label_the_result_of_linescan_register(Function *curFunction,InstNode * ins);
void label_register(Function *curFunction,InstNode*ins,Value *value,int i);
#endif //C22V1_LINE_SCAN_H
