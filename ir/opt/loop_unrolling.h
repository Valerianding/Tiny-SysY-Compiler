//
// Created by 12167 on 2023/7/1.
//

#ifndef C22V1_LOOP_UNROLLING_H
#define C22V1_LOOP_UNROLLING_H

#include "loopinfo.h"

void loop_unroll(Function *currentFunction);
//复制一个信息都相同的value副本
Value *copy_value(Value *v_source,int index);
//检查init、step、end是否都是int常数
bool check_idc(Loop* loop);
//计算迭代次数
int cal_times(int init,int step,int end);
//计算一次迭代的ir总数
int cal_ir_cnt(HashSet *loopBody);

bool check_in_latch_phi(Value *v,Value *variable,HashMap* latch_phi_map,HashMap* head_map);
Value *get_replace_value(HashSet* set,int block_id);
void update_replace_value(HashMap* map,Value* v_before,Value* v_replace);
void LOOP_UNROLL_EACH(Loop* loop);
void dfsLoop(Loop *loop);

#endif //C22V1_LOOP_UNROLLING_H
