//
// Created by 12167 on 2023/7/21.
//

#ifndef C22V1_LOOPUNROLL_H
#define C22V1_LOOPUNROLL_H

#include "loopinfo.h"
#include "instruction.h"
#include "bblock.h"
#include <math.h>

#define update_modifier 1
#define loop_unroll_up_lines 3000

void loop_unroll(Function *currentFunction);

#endif //C22V1_LOOPUNROLL_H
