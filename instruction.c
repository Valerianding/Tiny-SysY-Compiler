#include "instruction.h"


Instruction* ins_new(int op_num){
    int user_size = user_get_size(op_num);
    int use_size = user_size - sizeof(User);
    // use 是额外的，所以要单独计算大小 
    uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
    user_construct(storage, op_num);
    return (Instruction*)(storage+use_size);
}

Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2){
    Instruction* inst = ins_new(2);
    // 将 inst 这个 user 加入到 s1 和 s2 这两个 value 的 use_list
    Use* puse = user_get_operand_use(&inst->user, 0);
    value_add_use(S1, puse);

    puse = user_get_operand_use(&inst->user, 1);
    value_add_use(S1, puse);

    return inst;
}