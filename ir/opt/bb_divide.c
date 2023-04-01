//
// Created by Valerian on 2023/1/15.
//
#include "bb_divide.h"


void bblock_divide(InstNode *head){
    InstNode *cur = head;
    cur = get_next_inst(cur); //跳过第一个ALLBegin
    BasicBlock *globalBlock;
    InstNode *prev = cur;
    //如果下一个不是FuncBegin代表了有全局变量
    if(cur->inst->Opcode != FunBegin) {
        globalBlock = bb_create();
        globalBlock->head_node = cur;
        globalBlock->id = -1;

        //对全局变量设计一个同样的mem2reg的过程
        GlobalIncomingVal = HashMapInit();
        // 对所有的gloabl进行存储
        while (cur->inst->Opcode != FunBegin) {
            Value *globalAlloc = &cur->inst->user.value;
            stack *globalAllocStack = stackInit();

            // 存放的Value是多少
            Value *store = ins_get_lhs(cur->inst);

            printf("global alloc stored : %s\n",globalAlloc->name);
            printf("stored value : %s\n",store->name);
            //存进去
            stackPush(globalAllocStack,store);
            HashMapPut(GlobalIncomingVal,globalAlloc,globalAllocStack);


            ///修改Type 此处可能前端就处理了
            Value *insValue = ins_get_value(cur->inst);
            if(isIntType(insValue->VTy)){
                printf("changed a global type!\n");
                insValue->VTy->ID = GlobalVarInt;
            }else if(isFloatType(insValue->VTy)){
                printf("changed a global type!\n");
                insValue->VTy->ID = GlobalArrayFloat;
            }

            prev = cur;
            cur = get_next_inst(cur);
        }
        globalBlock->tail_node = prev;
        bb_set_block(globalBlock, globalBlock->head_node, globalBlock->tail_node);
    }

    //现在cur 为第一个函数开头
    //第一次全部打点
    InstNode *prev_in = NULL;
    int curBlockLabel = -1;
    while(cur != NULL){
        if(cur->inst->Opcode == FunBegin || cur->inst->Opcode == Label){
            InstNode *cur_prev = get_prev_inst(cur);
            if(cur_prev != NULL){
                cur_prev->inst->user.value.is_out = true;
            }
            cur->inst->user.value.is_in = true;
            prev_in = cur;

            if(cur->inst->Opcode == Label){
                curBlockLabel = cur->inst->user.value.pdata->instruction_pdata.true_goto_location;
            }else if(cur->inst->Opcode == FunBegin){
                curBlockLabel = 0;
            }
        }
        if(cur->inst->Opcode == br || cur->inst->Opcode == br_i1 || cur->inst->Opcode == FunEnd || cur->inst->Opcode == Return){
            cur->inst->user.value.is_out = true;
            InstNode *cur_next = get_next_inst(cur);
            if(cur_next != NULL){
                cur_next->inst->user.value.is_in = true;
            }
            BasicBlock *this = bb_create();
            this->id = curBlockLabel;
            bb_set_block(this,prev_in,cur);
        }
        cur = get_next_inst(cur);
    }
    cur = head;
    cur = get_next_inst(cur);
    Function *func_prev = nullptr;
    BasicBlock *entry = nullptr;

    while(cur != NULL){
        if(cur->inst->Opcode == FunBegin){
            entry = cur->inst->Parent;
        }
        if(cur->inst->Opcode == br || cur->inst->Opcode == br_i1 || cur->inst->Opcode == FunEnd || cur->inst->Opcode == Return) {
            BasicBlock *this = cur->inst->Parent;
            if (cur->inst->Opcode == FunEnd) {
                Function *cur_func = function_create();
                if (func_prev) {
                    func_prev->Next = cur_func;
                }
                func_prev = cur_func;
                printf("Set Function success !\n");
                func_set(cur_func, entry, this);
            } else if (cur->inst->Opcode == br) {
                InstNode *function_begin = get_func_start(cur);
                InstNode *true_label = search_ins_label(function_begin,
                                                        cur->inst->user.value.pdata->instruction_pdata.true_goto_location);
                BasicBlock *true_block = true_label->inst->Parent;
                bb_add_prev(this, true_block);
                this->true_block = true_block;
            } else if (cur->inst->Opcode == br_i1) {
                InstNode *function_begin = get_func_start(cur);
                InstNode *true_label = search_ins_label(function_begin,
                                                        cur->inst->user.value.pdata->instruction_pdata.true_goto_location);
                BasicBlock *true_block = true_label->inst->Parent;
                InstNode *false_label = search_ins_label(function_begin,
                                                         cur->inst->user.value.pdata->instruction_pdata.false_goto_location);
                BasicBlock *false_block = false_label->inst->Parent;
                bb_add_prev(this, true_block);
                bb_add_prev(this, false_block);
                this->true_block = true_block;
                this->false_block = false_block;
            }
        }
        cur = get_next_inst(cur);
    }
    printf("after second while\n");
}
