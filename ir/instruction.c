#include "instruction.h"
#include "stdio.h"

extern char t_num[5];
extern int t_index ;
extern char t[6];
extern Symtab *this;

//全局记录instruction的编号
int instruction_uid=0;

Instruction* ins_new(int op_num){
    int user_size = user_get_size(op_num);
    unsigned int use_size = user_size - sizeof(User);
    // use 是额外的，所以要单独计算大小 
    uint8_t *storage = (uint8_t *)malloc(sizeof(Instruction) + use_size);
    memset(storage, 0 , use_size);
    /* 为Instruction的Value初始化 */
    Instruction* ins = (Instruction*)(storage+use_size);
    value_init((Value*)ins);
    /* 在这里已经设置了指令的操作数个数 */
    user_construct(storage, op_num);
    return (Instruction*)(storage+use_size);
}

Instruction* ins_new_binary_operator(int Op, Value *S1, Value *S2){
    Instruction* inst = ins_new(2);
    // 将 inst 这个 user 加入到 s1 和 s2 这两个 value 的 use_list
    Use* pUse = user_get_operand_use(&inst->user, 0);
    use_set_value(pUse, S1);
    pUse = user_get_operand_use(&inst->user, 1);
    use_set_value(pUse, S2);

    inst->Opcode = Op;
    inst->i=instruction_uid;
    instruction_uid++;
    return inst;
}

Instruction *ins_new_unary_operator(int Op,Value *S1){
    Instruction* inst = ins_new(1);
    Use* pUse = user_get_operand_use(&inst->user,0);
    assert(pUse != NULL);
    use_set_value(pUse,S1);

    inst->Opcode = Op;
    inst->i = instruction_uid;
    instruction_uid++;
    return inst;
}

struct _BasicBlock *ins_get_parent(Instruction *ins){
    return ins->Parent;
}

//将临时变量%0 clear为%,或将%1 clear为%，后面跟的数量可重新赋值
void clear_tmp(char* tmp){
    char *p=tmp;
    p++;
    while(*p)
    {
        *p='\0';
        p++;
    }
}

//获取instruction.user.value
Value *ins_get_value(Instruction *ins){
    return &ins->user.value;
}

//获取instruction.user.value并赋个名字
Value *ins_get_value_with_name(Instruction *ins){
    Value *v_tmp = &ins->user.value;
    sprintf(t_num, "%d", t_index++);
    strcat(t,t_num);
    v_tmp->pdata->var_pdata.map_list = getCurMapList(this);
    v_tmp->name = (char*) malloc(strlen (t));
    strcpy(v_tmp->name,t);
    clear_tmp(t);
    return v_tmp;
}

//获取全局value并赋个name
Value *ins_get_global_value(Instruction *ins,char* name)
{
    Value *v_tmp = &ins->user.value;
    v_tmp->pdata->var_pdata.map_list = getCurMapList(this);
    v_tmp->name = (char*) malloc(strlen (name));
    strcpy(v_tmp->name,name);
    return v_tmp;
}

Value *ins_get_lhs(Instruction *ins){
    User *user = &ins->user;
    if(user->value.NumUserOperands == 0){
        return NULL;
    }
    Use *use1 = user_get_operand_use(user,0);
    Value *lhs = use1->Val;
    return lhs;
}

Value *ins_get_rhs(Instruction *ins){
    Value *dest = (Value*)ins;
    // 如果没有两个就报错
    if(dest->NumUserOperands != (unsigned int)2){
        return NULL;
    }
    User *user = &ins->user;
    Use *use2 = user_get_operand_use(user,1);
    Value *rhs = use2->Val;
    return rhs;
}

void ins_set_parent(Instruction *ins,struct _BasicBlock *parent){
    ins->Parent = parent;
}

void print_ins_opcode(Instruction *ins){
    switch (ins->Opcode) {
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
        case bitcast:
            printf("bitcast");
            break;
        case GEP:
            printf("getelementptr");
            break;
        case FunEnd:
            printf("FunEnd");
            break;
        default:
            printf("Normal Calculation");
            break;
    }
}
