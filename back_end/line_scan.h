//
// Created by Administrator on 2023/7/27.
//

#ifndef C22V1_LINE_SCAN_H
#define C22V1_LINE_SCAN_H
#include "live_interval.h"
PriorityQueue *get_function_live_interval(InstNode*ins);
void line_scan(InstNode*ins);
void line_scan_alloca();
#endif //C22V1_LINE_SCAN_H
