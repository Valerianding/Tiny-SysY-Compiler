//
// Created by Valerian on 2023/1/15.
//
#include "bb_divide.h"
extern HashMap* value_instruction_store; // 通过 instruction的value alloca(value)  ->  instruction  找到alloca指令对应的store
extern HashMap* value_instruction_load;  // 通过 instruction的value alloca(value)  ->  instructiond 找到alloca指令对应的load
void bblock_divide(InstNode *head){
    value_instruction_load = HashMapInit();
    value_instruction_store = HashMapInit();
    InstNode *cur = head;
    cur = get_next_inst(cur); //跳过第一个ALLBegin

    BasicBlock *globalBlock;
    InstNode *prev = cur;
    //如果下一个不是FuncBegin代表了有全局变量
    if(cur->inst->Opcode != FunBegin) {
        globalBlock = bb_create();
        globalBlock->head_node = cur;
        globalBlock->id = count;
        count++;
        cur = get_next_inst(cur);
        while (cur->inst->Opcode != FunBegin) {
            prev = cur;
            cur = get_next_inst(cur);
        }
        globalBlock->tail_node = prev;
        bb_set_block(globalBlock, globalBlock->head_node, globalBlock->tail_node);
    }


    //现在cur 为第一个函数开头
    //第一次全部打点
    //printf("first while\n");
    InstNode *prev_in;
    while(cur != NULL){
        if(cur->inst->Opcode == FunBegin || cur->inst->Opcode == Label){
            InstNode *cur_prev = get_prev_inst(cur);
            if(cur_prev != NULL){
                cur_prev->inst->user.value.is_out = true;
            }
            cur->inst->user.value.is_in = true;
            prev_in = cur;
        }
        if(cur->inst->Opcode == Return || cur->inst->Opcode == br || cur->inst->Opcode == br_i1){
            cur->inst->user.value.is_out = true;
            InstNode *cur_next = get_next_inst(cur);
            if(cur_next != NULL){
                cur_next->inst->user.value.is_in = true;
            }
            BasicBlock *this = bb_create();
            this->id = count;
            count++;
            bb_set_block(this,prev_in,cur);
        }
        //为后面的mem2reg打基础
        if(cur->inst->Opcode == Load){
            //找到load的value 只有可能有一个
            Value *load_value = cur->inst->user.use_list->Val;
            HashMapPut(value_instruction_load,load_value,cur);
        }
        if(cur->inst->Opcode == Store){
            //找到store的value  应该对应的是第二个value
            Value *store_value = cur->inst->user.use_list[1].Val;
            HashMapPut(value_instruction_store,store_value,cur);
        }
        cur = get_next_inst(cur);
    }
    cur = head;
    cur = get_next_inst(cur);
    Function *func_prev = nullptr;
    BasicBlock *entry = nullptr;
    //printf("second while\n");
    //第二次直接套壳
    while(cur != NULL){
        if(cur->inst->Opcode == FunBegin){
            entry = cur->inst->Parent;
        }
        if(cur->inst->Opcode == Return || cur->inst->Opcode == br || cur->inst->Opcode == br_i1) {
            BasicBlock *this = cur->inst->Parent;
            if (cur->inst->Opcode == Return) {
                Function *cur_func = function_create();
                if (func_prev) {
                    func_prev->Next = cur_func;
                }
                func_prev = cur_func;
                printf("Set Function sucess !\n");
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
        if(cur->inst->Opcode == Alloca){
            Value *alloca_value = &cur->inst->user.value;
            //if(!HashMapContain(value_instruction_load,alloca_value)){
//                //如果对于一个alloca都没有load的话，我们不需要这句alloca
//                InstNode *pInstNode = get_prev_inst(cur);
//                InstNode *nInstNode = get_next_inst(cur);
//                if(pInstNode && nInstNode){
//                    pInstNode->list.next = &nInstNode->list;
//                    nInstNode->list.next = &pInstNode->list;
//                }
//            }
            //            Value *alloca_value = cur->inst->user.use_list->Val;
//            if(!HashMapContain(value_instruction_load,alloca_value)){
//                //如果一个alloca都没有load的话 那我们就不需要这句alloca了
//                //同样的对于它进行的store指令也都是无效的了
//                // 这里我们没有考虑主函数的
//                InstNode *prev = get_prev_inst(cur);
//                InstNode *next = get_next_inst(cur);
//                if (prev && next) {
//                    prev->list.next = &next->list;
//                    next->list.prev = &prev->list;
//                }


            //找到对应的store语句 也全部进行删除
        }
        cur = get_next_inst(cur);
    }
}
