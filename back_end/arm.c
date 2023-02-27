//
// Created by tom on 23-2-25.
//
#include "arm.h"
//InstNode *head=NULL;



//int i_ins=0;
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
//    这个是需要取到alias里面保存的%i的i值
    return 0;
}

void get_value_name(Value*value,char name[]){
    name=value->name;
    return;
}
void arm_translate_ins(InstNode *ins){
//    int x=1;
    InstNode *head=(InstNode*) malloc(sizeof(InstNode*));
    for(;ins!=NULL;ins= get_next_inst(ins)) {
//        printf("%d\n",x++);
        if(ins->inst->Opcode==FunBegin){
            head=ins;
        }
        ins=_arm_translate_ins(ins,head);
    }
    free(head);
    return;
}
InstNode * arm_trans_Add(InstNode *ins){
//    假设两个数都存在内存中，之后要考虑a+15这种立即书的情况

    printf("    add r0,r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==Store){
        ins=temp;
        int y= user_get_operand_use(&ins->inst->user,1)->Val->pdata->var_pdata.iVal;
        printf("    str r0,[sp,#%d]\n",y);
    }

    return  ins;

}
InstNode * arm_trans_Sub(InstNode *ins){

    printf("    sub r0,r0,r1\n");
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==Store){
        ins=temp;
        int y= user_get_operand_use(&ins->inst->user,1)->Val->pdata->var_pdata.iVal;
        printf("    str r0,[sp,#%d]\n",y);
    }
    //    printf("%d: str,r0,[sp,#%d]\n",i,x3);
    return  ins;
}
InstNode * arm_trans_Mul(InstNode *ins){

    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==Store){
        ins=temp;
        int y= user_get_operand_use(&ins->inst->user,1)->Val->pdata->var_pdata.iVal;
        printf("    str r0,[sp,#%d]\n",y);
    }
    printf("    mul r0,r1,r2\n");
    //    printf("%d: str,r0,[sp,#%d]\n",i,x3);
    return  ins;
}
InstNode * arm_trans_Div(InstNode *ins){

//除法指令需要跳转到相应的微处理程序
//    printf("\n");
    return ins;

}
InstNode * arm_trans_Module(InstNode *ins){
    printf("\n");
    return ins;
}
InstNode * arm_trans_Call(InstNode *ins){
//    printf("CALL\n");
    printf("    bl %s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    return ins;
}
InstNode * arm_trans_FunBegin(InstNode *ins){

    printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
//    printf("    bl  %s\n",user_get_operand_use(&ins->inst->user,0)->Val->name);
    if(!strcmp("main", user_get_operand_use(&ins->inst->user,0)->Val->name)){
        printf("    push {r11,lr}\n");
        printf("    mov r1,sp\n");
    }
    int x= get_siezof_sp(ins);
    printf("    sub sp,sp,#%d\n",x);
    return ins;
}
InstNode * arm_trans_Return(InstNode *ins,InstNode *head){
//    return语句的翻译流程是将要返回的参数存放在r0里面，之后进行栈帧的调整工作


    if(!strcmp("main", user_get_operand_use(&head->inst->user,0)->Val->name)){
        printf("    mov r11,sp\n");
        printf("    pop {r11,lr}\n");
        printf("    bx lr\n");
        return ins;
    }
    int x1= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;

    printf("    ldr r0,[sp,#%d]\n",x1);
    x1= get_siezof_sp(head);
//    printf("%s\n", user_get_operand_use(&head->inst->user,0)->Val->name);
//    printf("    mov sp,r11\n");
//    printf("    pop {r11,lr}\n");
    printf("    add sp,sp,#%d\n",x1);
    printf("    bx lr\n");
    return ins;
}
InstNode * arm_trans_Store(InstNode *ins){

//这是立即数的翻译方案就是a=5，另外还有一种a=b的情况，
    int x= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
    int y= user_get_operand_use(&ins->inst->user,1)->Val->pdata->var_pdata.iVal;
//    这个跳转的位置是存在value2的那个地方
    printf("    mov r0,#%d\n",x);
//  r0?
    printf("    str r0,[sp,#%d]\n",y);
    return ins;
}
InstNode * arm_trans_Load(InstNode *ins){
//    int i=ins->inst->i;
    int x=0;
    printf("    ldr r0,[sp,#%d]\n",x);
//    printf("***************LOAD1\n");
    if(get_next_inst(ins)->inst->Opcode==Load){
        ins= get_next_inst(ins);
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
    return ins;
}
InstNode * arm_trans_Alloca(InstNode *ins){

    return ins;
}
InstNode * arm_trans_GIVE_PARAM(InstNode *ins){
//  这个是参数传递的，暂时先不处理
    printf("\n");
    return ins;
}
InstNode * arm_trans_ALLBEGIN(InstNode *ins){
//    int i=ins->inst->i;
    printf("**********ALLBEGIN**************\n");
    return ins;
}
InstNode * arm_trans_LESS(InstNode *ins){
//a<b  a,b
    printf("    cmp r0,r1\n");
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
    printf("    cmp r0,r1\n");
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
    printf("    cmp r0,r1\n");
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
    printf("    cmp r0,r1\n");
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
    printf("    cmp r0,r1\n");
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
    printf("    cmp r0,r1\n");
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

//    printf("\n");
    return ins;
}
InstNode * arm_trans_br_i1_false(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_Label(InstNode *ins){

    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("%d：\n",x);
    return ins;
}
InstNode * arm_trans_tmp(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_XOR(InstNode *ins){
//    printf("\n");
    return ins;
}
InstNode * arm_trans_zext(InstNode *ins){
//    printf("\n");
    return ins;
}
InstNode * arm_trans_bitcast(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_GMP(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_MEMCPY(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_zeroinitializer(InstNode *ins){

//    printf("\n");
    return ins;
}
InstNode * arm_trans_GLOBAL_VAR(InstNode *ins){

//    printf("\n");
    return ins;
}

InstNode *_arm_translate_ins(InstNode *ins,InstNode *head){
    int x=ins->inst->Opcode;
    switch(x){
        case Add:
            return arm_trans_Add(ins);
            break;
        case Sub:
            return arm_trans_Sub(ins);
            break;
        case Mul:
            return arm_trans_Mul(ins);
            break;
        case Div:
            return arm_trans_Div(ins);
            break;
        case Module:
            return arm_trans_Module(ins);
            break;
        case Call:
            return arm_trans_Call(ins);
            break;
        case FunBegin:
//            head =ins;
            return arm_trans_FunBegin(ins);
            break;
        case Return:
            return arm_trans_Return(ins,head);
            break;
        case Store:
            return arm_trans_Store(ins);
            break;
        case Load:
            return arm_trans_Load(ins);
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
            return arm_trans_GMP(ins);
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
    }
}
