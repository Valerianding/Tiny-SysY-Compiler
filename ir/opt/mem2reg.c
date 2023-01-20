//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"
HashMap* value_instruction_store; // alloca(value)  ->  instruction
HashMap* value_instruction_load;  //

void delete_useless_store(InstNode *head,Value *value){
    InstNode *cur = head;
    while(cur != nullptr){

        
        cur = get_next_inst(cur);
    }
}