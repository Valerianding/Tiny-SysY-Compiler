#include "instruction.h"
#include "stdio.h"

//全局记录instruction的编号
int instruction_uid=0;

Instruction* ins_new(int op_num){
    int user_size = user_get_size(op_num);
    int use_size = user_size - sizeof(User);
    // use 是额外的，所以要单独计算大小 
    uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
    /* 为Instruction的Value初始化 */
    Instruction* temp = (Instruction*)(storage+use_size);
    value_init((Value*)temp);
    /* 在这里已经设置了指令的操作数个数 */
    user_construct(storage, op_num);
    return (Instruction*)(storage+use_size);
}

Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2){
    Instruction* inst = ins_new(2);
    // 将 inst 这个 user 加入到 s1 和 s2 这两个 value 的 use_list
    Use* puse = user_get_operand_use(&inst->user, 0);
    use_set_value(puse, S1);
    puse = user_get_operand_use(&inst->user, 1);
    use_set_value(puse, S2);

    inst->Opcode = Op;
    inst->i=instruction_uid;
    instruction_uid++;
    //inst->user.value.IsInstruction = 1;
    return inst;
}

Instruction *ins_new_unary_operator(int Op,Value *S1){
    Instruction* inst = ins_new(1);
    Use* puse = user_get_operand_use(&inst->user,0);
    use_set_value(puse,S1);

    inst->Opcode = Op;
    inst->i=instruction_uid;
    instruction_uid++;
    //inst->user.value.IsInstruction = 1;
    return inst;
}

Instruction *ins_new_goto_operator(int pos){
    Instruction *inst = ins_new(0);
    inst->Opcode = Goto;
    inst->user.value.pdata->instruction_pdata.goto_location = pos;
    return inst;
}

Instruction *ins_new_ifgoto_operator(int pos,Value *S1,Value *S2){
    Instruction *inst;
    if(S2 == NULL){
        inst = ins_new(1);
        Use* puse = user_get_operand_use(&inst->user,0);
        use_set_value(puse,S1);
    }else{
        inst = ins_new(2);
        Use* puse = user_get_operand_use(&inst->user,0);
        use_set_value(puse,S1);
        puse = user_get_operand_use(&inst->user,1);
        use_set_value(puse,S2);
    }
    inst->user.value.pdata->instruction_pdata.goto_location = pos;
    inst->Opcode = IF_Goto;
}

struct _BasicBlock *ins_get_parent(Instruction *this){
    return this->Parent;
}

Instruction *ins_set_parent(Instruction *this,struct _BasicBlock *parent){
    this->Parent = parent;
}
