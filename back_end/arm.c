//
// Created by tom on 23-2-25.
//
#include "arm.h"
int regi=0;

bool is_int(Value*value){
    return isIntType(value->VTy);
}
bool is_float(Value*value){
    return isFloatType(value->VTy);
}
int get_value_offset(offset*head,Value*value){
    int i=-1;
    for(;head!=NULL;head=head->next){
        i++;
        if(head->value==value){
            return i*4;
        }
    }
    return -1;
}
int get_siezof_sp(InstNode*ins){
    int x=0;
    ins= get_next_inst(ins);
    for(;ins!=NULL;ins= get_next_inst(ins)){
        int opcode=ins->inst->Opcode;
        if(opcode==Alloca)
            x+=1;
        if(opcode==FunBegin)
            break;
    }
    return x*4;
}
int get_value_pdata_inspdata_true(Value*value){
    return value->pdata->instruction_pdata.true_goto_location;
}
int get_value_pdata_inspdata_false(Value*value){
    return value->pdata->instruction_pdata.false_goto_location;
}
int get_value_pdata_varpdata_ival(Value*value){
    return value->pdata->var_pdata.iVal;
}
float get_value_pdata_varpdata_fval(Value*value){
    return value->pdata->var_pdata.iVal;
}
int get_value_alias(Value*value){
    return 0;
}
void get_value_name(Value*value,char name[]){
    name=value->name;
    return;
}
void arm_translate_ins(InstNode *ins){
//    int x=1;
    InstNode *head=(InstNode*) malloc(sizeof(InstNode*));
    offset *offset_head=NULL;
    for(;ins!=NULL;ins= get_next_inst(ins)) {
//        printf("%d\n",x++);
        if(ins->inst->Opcode==FunBegin){
            head=ins;
//            在进入函数时将offset初始化好，以供load指令使用
            offset_head= offset_init(ins);
            regi=0;

        }
        if(ins->inst->Opcode==Return){

            offset_head_free(offset_head);
            offset_head=NULL;

        }
        ins=_arm_translate_ins(ins,head,offset_head);
    }

    return;
}
InstNode * arm_trans_Add(InstNode *ins,offset*offset_head){

    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    Value value0=ins->inst->user.value;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    add r0,#%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        printf("    add r0,#%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
        int x2=value2->pdata->var_pdata.iVal;
        printf("    add r0,r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        printf("    add r0,r0,r1\n");
    }

    return  ins;

}
InstNode * arm_trans_Sub(InstNode *ins,offset*offset_head){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;

    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    sub r0,#%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        printf("    sub r0,#%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
        int x2=value2->pdata->var_pdata.iVal;
        printf("    sub r0,r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        printf("    sub r0,r0,r1\n");
    }
    return  ins;
}
InstNode * arm_trans_Mul(InstNode *ins,offset*offset_head){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    mul r0,#%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        printf("    mul r0,#%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
        int x2=value2->pdata->var_pdata.iVal;
        printf("    mul r0,r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        printf("    mul r0,r0,r1\n");
    }
    return  ins;
}
InstNode * arm_trans_Div(InstNode *ins,offset*offset_head){

    printf("\n");
    return ins;

}
InstNode * arm_trans_Module(InstNode *ins){
    printf("arm_trans_Module\n");
    return ins;
}
InstNode * arm_trans_Call(InstNode *ins){
//    printf("CALL\n");
    printf("    bl %s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    return ins;
}
InstNode * arm_trans_FunBegin(InstNode *ins,offset*offset_head){

    printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);

    if(!strcmp("main", user_get_operand_use(&ins->inst->user,0)->Val->name)){
//        printf("hehe\n");
        printf("    push {r11,lr}\n");
        printf("    mov r1,sp\n");
    }
    int x= get_siezof_sp(ins);
    printf("    sub sp,sp,#%d\n",x);
    int param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
    if(param_num!=0){
        ;
//              进行含参数函数的store指令的处理
        ins= get_next_inst(ins);
        for(;ins->inst->Opcode==Alloca;ins= get_next_inst(ins));

        for(int i=0;i<param_num;i++){
            int x=get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,1)->Val->alias);
            printf("    str r%d,[sp,#%d]\n",i,x);
            ins= get_next_inst(ins);
        }
    }
    return ins;
}
InstNode * arm_trans_Return(InstNode *ins,InstNode *head){

    if(!strcmp("main", user_get_operand_use(&head->inst->user,0)->Val->name)){
        printf("    mov r11,sp\n");
        printf("    pop {r11,lr}\n");
        printf("    bx lr\n");
        return ins;
    }

    int x1= get_siezof_sp(head);

    printf("    add sp,sp,#%d\n",x1);
    printf("    bx lr\n");
    return ins;
}
InstNode * arm_trans_Store(InstNode *ins,offset*offset_head){

    if(isImm(user_get_operand_use(&ins->inst->user,0)->Val)){
        printf("    mov r0,#%d\n",user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal);
        int x=get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,1)->Val->alias);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,1)->Val->alias->name);
        printf("    str r0,[sp,#%d]\n",x);
    }
    else{
        int x=get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,1)->Val->alias);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,1)->Val->alias->name);
            printf("    str r0,[sp,#%d]\n",x);
    }

    return ins;
}
InstNode * arm_trans_Load(InstNode *ins,offset*offset_head){
    if(get_next_inst(ins)->inst->Opcode==GIVE_PARAM){
        int x= get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,0)->Val->alias);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->alias->name);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->name);
        printf("    ldr r%d,[sp,#%d]\n",regi++,x);
    }
//    int i=ins->inst->i;
    else{
        int x= get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,0)->Val->alias);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->alias->name);
//        printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->name);
        printf("    ldr r0,[sp,#%d]\n",x);
//    printf("***************LOAD1\n");
        if(get_next_inst(ins)->inst->Opcode==Load){
            ins= get_next_inst(ins);
//            printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->alias->name);
//            printf("%s\n",user_get_operand_use(&ins->inst->user,0)->Val->name);
            x= get_value_offset(offset_head,user_get_operand_use(&ins->inst->user,0)->Val->alias);
            printf("    ldr r1,[sp,#%d]\n",x);
//        printf("***************LOAD2\n");
            InstNode *temp= get_next_inst(ins);
            if(temp->inst->Opcode==LESSEQ){
                ins=temp;
                ins= arm_trans_LESSEQ(ins);
            }
            if(temp->inst->Opcode==LESS){
                ins=temp;
                ins= arm_trans_LESS(ins);
            }
            if(temp->inst->Opcode==GREAT){
                ins=temp;
                ins= arm_trans_GREAT(ins);
            }
            if(temp->inst->Opcode==GREATEQ){
                ins=temp;
                ins= arm_trans_GREATEQ(ins);
            }
            if(temp->inst->Opcode==EQ){
                ins=temp;
                ins= arm_trans_EQ(ins);
            }
            if(temp->inst->Opcode==NOTEQ){
                ins=temp;
                ins= arm_trans_NOTEQ(ins);
            }
        }
    }
//    printf("Load success\n");
    return ins;
}
InstNode * arm_trans_Alloca(InstNode *ins){
//    在汇编中，alloca不需要翻译。
    return ins;
}
InstNode * arm_trans_GIVE_PARAM(InstNode *ins){
//  这个是用来标定参数传递的，仅仅是一个指示作用
//    printf("\n");
    return ins;
}
InstNode * arm_trans_ALLBEGIN(InstNode *ins){
//    int i=ins->inst->i;
    printf("**********ALLBEGIN**************\n");
    return ins;
}
InstNode * arm_trans_LESS(InstNode *ins){
//a<b  a,b
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }
//    printf("    cmp r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    bge %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;


}
InstNode * arm_trans_GREAT(InstNode *ins){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    ble %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;
//    printf("\n");
//    return ins;
}
InstNode * arm_trans_LESSEQ(InstNode *ins){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }
//    printf("    cmp r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    bgt %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;

//    printf("\n");
//    return ins;
}
InstNode * arm_trans_GREATEQ(InstNode *ins){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }

//    printf("    cmp r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    blt %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;
//    printf("\n");
//    return ins;
}
InstNode * arm_trans_EQ(InstNode *ins){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }
//    printf("    cmp r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
//        printf("***************EQ\n");
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    bne %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;
//    printf("\n");
//    return ins;
}
InstNode * arm_trans_NOTEQ(InstNode *ins){
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImm(value1)&& isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,#%d\n",x1,x2);
    }
    if(isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
//        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp #%d,r0\n",x1);
    }
    if(!isImm(value1)&& isImm(value2)){
//        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,#%d\n",x2);
    }
    if(!isImm(value1)&& !isImm(value2)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        printf("    cmp r0,r1\n");
    }
//    printf("    cmp r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==br_i1){
        ins= temp;
        int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
        printf("    beq %d\n",x);
        x= get_value_pdata_inspdata_true(&ins->inst->user.value);
        printf("    b %d\n",x);
    }
    return ins;
//    printf("\n");
//    return ins;
}
InstNode * arm_trans_br_i1(InstNode *ins){
//    int i=ins->inst->i;
    int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
    printf("    bne %d\n",x);
    x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("    b %d\n",x);
    return  ins;
}
InstNode * arm_trans_br(InstNode *ins){
//    int i=ins->inst->i;
//    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("    b %d\n",x);
    return ins;
}
InstNode * arm_trans_br_i1_true(InstNode *ins){

    printf("arm_trans_br_i1_true\n");
    return ins;
}
InstNode * arm_trans_br_i1_false(InstNode *ins){

    printf("arm_trans_br_i1_false\n");
    return ins;
}
InstNode * arm_trans_Label(InstNode *ins){
//强制跳转的位置
    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("%d：\n",x);
    return ins;
}
InstNode * arm_trans_tmp(InstNode *ins){

    printf("arm_trans_tmp\n");
    return ins;
}
InstNode * arm_trans_XOR(InstNode *ins){
    printf("arm_trans_XOR\n");
    return ins;
}
InstNode * arm_trans_zext(InstNode *ins){
//i1扩展为i32
    printf("arm_trans_zext\n");
    return ins;
}
InstNode * arm_trans_bitcast(InstNode *ins){
//类型转换
    printf("arm_trans_bitcast\n");
    return ins;
}
InstNode * arm_trans_GMP(InstNode *ins,offset*head){
//数组初始化
    if(get_next_inst((ins))->inst->Opcode==Store){
        InstNode *next= get_next_inst(ins);
        int off= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
        int x= user_get_operand_use(&next->inst->user,0)->Val->pdata->var_pdata.iVal;
        printf("    mov r0,#%d\n",x);
        int off_sp= get_value_offset(head, user_get_operand_use(&next->inst->user,1)->Val->alias)+off;
        printf("    store r0,[sp,#%d]\n",off_sp*4);
        return next;
    }
//    printf("\n");
    return ins;
}
InstNode * arm_trans_MEMCPY(InstNode *ins){
//    涉及到数组的内容
    printf("arm_trans_MEMCPY\n");
    return ins;
}
InstNode * arm_trans_zeroinitializer(InstNode *ins){

    printf("arm_trans_zeroinitializer\n");
    return ins;
}
InstNode * arm_trans_GLOBAL_VAR(InstNode *ins){
//全局变量声明
    printf("arm_trans_GLOBAL_VAR\n");
    return ins;
}
InstNode *arm_trans_Phi(InstNode *ins){
    printf("arm_trans_Phi\n");
    return ins;
}
InstNode *arm_trans_MEMSET(InstNode *ins){
    printf("arm_trans_MEMSET\n");
    return ins;
}

InstNode *_arm_translate_ins(InstNode *ins,InstNode *head,offset*offset_head){
    int x=ins->inst->Opcode;
    switch(x){
        case Add:
            return arm_trans_Add(ins,offset_head);
            break;
        case Sub:
            return arm_trans_Sub(ins,offset_head);
            break;
        case Mul:
            return arm_trans_Mul(ins,offset_head);
            break;
        case Div:
            return arm_trans_Div(ins,offset_head);
            break;
        case Module:
            return arm_trans_Module(ins);
            break;
        case Call:
            return arm_trans_Call(ins);
            break;
        case FunBegin:
//            head =ins;
            return arm_trans_FunBegin(ins,offset_head);
            break;
        case Return:
            return arm_trans_Return(ins,head);
            break;
        case Store:
            return arm_trans_Store(ins,offset_head);
            break;
        case Load:
            return arm_trans_Load(ins,offset_head);
            break;
        case Alloca:
            return arm_trans_Alloca(ins);
            break;
        case GIVE_PARAM:
            return arm_trans_GIVE_PARAM(ins);
            break;
        case ALLBEGIN:
            return arm_trans_ALLBEGIN(ins);
            break;
        case LESS:
            return arm_trans_LESS(ins);
            break;
        case GREAT:
            return arm_trans_GREAT(ins);
            break;
        case LESSEQ:
            return arm_trans_LESSEQ(ins);
            break;
        case GREATEQ:
            return arm_trans_GREATEQ(ins);
            break;
        case EQ:
            return arm_trans_EQ(ins);
            break;
        case NOTEQ:
            return arm_trans_NOTEQ(ins);
            break;
        case br_i1:
            return arm_trans_br_i1(ins);
            break;
        case br:
            return arm_trans_br(ins);
            break;
        case br_i1_true:
            return arm_trans_br_i1_true(ins);
            break;
        case br_i1_false:
            return arm_trans_br_i1_false(ins);
            break;
        case Label:
            return arm_trans_Label(ins);
            break;
        case tmp:
            return arm_trans_tmp(ins);
            break;
        case XOR:
            return arm_trans_XOR(ins);
            break;
        case zext:
            return arm_trans_zext(ins);
            break;
        case bitcast:
            return arm_trans_bitcast(ins);
            break;
        case GMP:
            return arm_trans_GMP(ins,offset_head);
            break;
        case MEMCPY:
            return arm_trans_MEMCPY(ins);
            break;
        case zeroinitializer:
            return arm_trans_zeroinitializer(ins);
            break;
        case GLOBAL_VAR:
            return arm_trans_GLOBAL_VAR(ins);
            break;
        case Phi:
            return arm_trans_Phi(ins);
            break;
        case MEMSET:
            return arm_trans_MEMSET(ins);
            break;
    }
}
