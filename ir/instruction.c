#include "instruction.h"
#include "stdio.h"

extern char t_num[3];
extern int t_index ;
extern char t[5];
extern Symtab *this;

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

struct _BasicBlock *ins_get_parent(Instruction *this){
    return this->Parent;
}

//将临时变量t0 clear为t,或将%1 clear为%，后面跟的数量可重新赋值
void clear_tmp(char* tmp)
{
    char *p=tmp;
    p++;
    while(*p)
    {
        *p='\0';
        p++;
    }
}

//获取instruction.user.value并赋个名字
Value *ins_get_value(Instruction *ins){
    return &ins->user.value;
}

//获取instruction.user.value并赋个名字
Value *ins_get_value_with_name(Instruction *ins){
    Value *v_tmp=&ins->user.value;
    sprintf(t_num, "%d", t_index++);
    strcat(t,t_num);
    v_tmp->pdata->var_pdata.map= getCurMap(this);
    v_tmp->name=(char*) malloc(strlen (t));
    strcpy(v_tmp->name,t);
    clear_tmp(t);
    return v_tmp;
}

Instruction *ins_set_parent(Instruction *this,struct _BasicBlock *parent){
    this->Parent = parent;
}

void print_ins_opcode(Instruction *this){
    switch (this->Opcode) {
        case Store:
            printf("Store");
            break;
        case Load:
            printf("Load");
            break;
        case Alloca:
            printf("Alloca");
            break;
        case Call:
            printf("Call");
            break;
        case FunBegin:
            printf("FuncBegin");
            break;
        case Return:
            printf("Return");
            break;
        case GIVE_PARAM:
            printf("GIVE_PARAM");
            break;
        case ALLBEGIN:
            printf("ALLBegin");
            break;
        case br:
            printf("Br");
            break;
        case br_i1:
            printf("Br_i1");
            break;
        case Label:
            printf("Label");
            break;
        default:
            printf("Normal Calculation");
            break;
    }
}
