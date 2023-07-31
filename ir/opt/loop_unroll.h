//
// Created by 12167 on 2023/7/31.
//

#ifndef C22V1_LOOP_UNROLL_H
#define C22V1_LOOP_UNROLL_H

#include "loopinfo.h"
#include "instruction.h"
#include "bblock.h"
#include <math.h>

#define update_modifier 4
#define loop_unroll_up_lines 3000

void loop_unroll(Function *currentFunction);

#endif //C22V1_LOOP_UNROLL_H
