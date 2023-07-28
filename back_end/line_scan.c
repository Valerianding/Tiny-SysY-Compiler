//
// Created by Administrator on 2023/7/27.
//
#include "line_scan.h"

//传入的是FuncBegin的ins
PriorityQueue *get_function_live_interval(InstNode*ins){
    BasicBlock *block;
    Function *function;
    block=ins->inst->Parent;
    function=block->Parent;

    PriorityQueue *queue;
    queue= build_live_interval(function->ToPoBlocks);
    print_live_interval();
    return queue;
}

void line_scan_alloca(InstNode*ins,PriorityQueue*queue){

}

void line_scan(InstNode*ins){
    PriorityQueue *queue;
    while (ins!=NULL){
        if(ins->inst->Opcode==FunBegin){
            printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
            queue= get_function_live_interval(ins);
            line_scan_alloca(ins,queue);
        }
        ins= get_next_inst(ins);
    }
}