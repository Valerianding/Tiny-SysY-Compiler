//
// Created by tom on 23-2-25.
//
#include "arm.h"

//ri
int regi=0;
//si
int regs=0;
int func_call_func;
bool imm_is_valid(unsigned int imm){
    int i;
    for (i = 0; i <= 30; i += 2) {
        unsigned int rotated = (imm >> i) | (imm << (32 - i));
        if (rotated <= 255 && (rotated & ~0xff) == 0)
            return true;
    }
    return false;
}
FILE *open_file(char argv[]){
    int len= strlen(argv);
    char filepath[50];
    char *prefix= strncpy(malloc(len-2),argv,len-3);
    prefix[len-3]='\0';
    snprintf(filepath, sizeof(filepath),"../test/%s.s",prefix);
    free(prefix);
    FILE *myopen= fopen(filepath,"w");
//    fprintf(myopen,"haha");
    if(myopen==NULL){
        perror("open file error");
    }
    return myopen;
}

int get_value_offset_sp(HashMap *hashMap,Value*value){
    offset *node= HashMapGet(hashMap, value);
    if(node!=NULL){
        return node->offset_sp;
    }
    return -1;
}

void FuncBegin_hashmap_add(HashMap*hashMap,Value *value,char *name,int *local_stack){
// 这种存储方法好像是错的，因为后面用到的同一个value，这里给他分配了不同地址的mykey_node()，所以回得到不的key
// hashmap不会释放value*对应的内存

//    if(value->VTy->ID==ArrayTyID){
////        是数组，offset_sp需要加更多
//    }
//    是参数,不用处理,这里是用来计算局部变量的栈开多大，再加上param_num*4就可以得到总的栈帧的大小了。
    if(value->name!=NULL && strcmp(value->name,name)<0 && strlen(value->name)<= strlen(name)){
        ;
//        free(key);重复释放的原因有可能在这里
    } else{
        if(!HashMapContain(hashMap, value)){
            offset *node=offset_node();
            node->memory=1;
            node->regs=-1;
            node->regr=-1;
            node->offset_sp=*local_stack;
            (*local_stack)+=4;
//            数组只在alloca指令里面开辟，其他的不用管
//            if(isLocalArrayIntType(value->VTy)||isLocalArrayFloatType(value->VTy)||isGlobalArrayIntType(value->VTy)||isGlobalArrayFloatType(value->VTy)){
////        是数组，offset_sp需要加更多
//                int x= get_array_total_occupy(value->alias,0);
//                (*local_stack)=(*local_stack)+x-4;
//            }
//            printf("funcBeginhaspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,value->name,value);
            HashMapPut(hashMap,value,node);
        }
    }
    return;
}
void FuncBegin_hashmap_alloca_add(HashMap*hashMap,Value *value,int *local_stack){
    if(!HashMapContain(hashMap, value)){
//        offset *node=offset_node();
//        node->memory=1;
//        node->regs=-1;
//        node->regr=-1;
//        node->offset_sp=*local_stack;
//        (*local_stack)+=4;
//        HashMapPut(hashMap,value,node);
        if(isLocalArrayIntType(value->VTy)||isLocalArrayFloatType(value->VTy)||isGlobalArrayIntType(value->VTy)||isGlobalArrayFloatType(value->VTy)){
            int size_array= get_array_total_occupy(value->alias,0);
            offset *node=offset_node();
            node->offset_sp=(*local_stack);
            (*local_stack)+=size_array;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,value,node);
        } else{
            offset *node=offset_node();
            node->offset_sp=(*local_stack);
            (*local_stack)+=4;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,value,node);
        }
    }
    return;
}
void FuncBegin_hashmap_bitcast_add(HashMap*hashMap,Value *value0,Value *value1,int *local_stack){
//    Value *contain=value1;
//    while (HashMapContain(hashMap,contain)){
//
//    }
    if(!HashMapContain(hashMap,value0)){
        if(HashMapContain(hashMap,value1)){
            offset *node= HashMapGet(hashMap,value1);
            HashMapPut(hashMap,value0,node);
        }
    }
    return;

//    offset *node=offset_node();
//    node->offset_sp=(*local_stack);
//    (*local_stack)+=4;
//    node->memory=true;
//    node->regs=-1;
//    node->regr=-1;
//    HashMapPut(hashMap,value0,node);
}

int get_siezof_sp(HashMap*hashMap){
    return HashMapSize(hashMap);
}

int get_value_pdata_inspdata_true(Value*value){
    return value->pdata->instruction_pdata.true_goto_location;
}

int get_value_pdata_inspdata_false(Value*value){
    return value->pdata->instruction_pdata.false_goto_location;
}

void arm_translate_ins(InstNode *ins){
    InstNode *head;
    HashMap *hashMap;
    int stack_size=0;
    for(;ins!=NULL;ins=get_next_inst(ins)) {
        if(ins->inst->Opcode==FunBegin){
//  这个逻辑有问题，现在的设计是再FunBegin里面执行栈帧开辟的sub指令，大小就是局部变量个数加上参数个数*4
//  然后在sub指令之后执行str r0,r1,r2,r3就是保存这几个寄存器的值，保存的位置就是在sp+局部变量个数*4+参数序号(从0开始)*4
//  执行完这两个之后，返回局部变量开辟的栈帧大小。返回之后进行
//  真正栈帧的开辟，参数的存储位置从局部变量位置之上开始存储。
//  所以说当前这个逻辑和FuncBegin函数的实现逻辑都得大改。
            regi=0;
            regs=0;
            stack_size=0;
            func_call_func=0;
            ins= arm_trans_FunBegin(ins,&stack_size);
            head=ins;
            int  param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
            int local_var_num=(stack_size-param_num*4)/4;
//            在进入函数时将offset初始化好，以供load指令使用
            hashMap=offset_init(ins,&local_var_num);
            int hashsize= HashMapSize(hashMap)*4;
//            printf("hashsize=%d\n",hashsize);
            ins= get_next_inst(ins);
        }
        ins=_arm_translate_ins(ins,head,hashMap,stack_size);
        if(ins->inst->Opcode==Return){
            offset_free(hashMap);
            hashMap=NULL;
        }
    }

    return;
}
InstNode * arm_trans_CopyOperation(InstNode*ins,HashMap*hashMap){
//    默认左值的type默认和value1的type是一样的,需要考虑value0和value1是否在寄存器或者说是内存里面，这个可以后面在完善的，现在先把样例过了
    Value *value0=ins->inst->user.value.alias;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int x0= get_value_offset_sp(hashMap,value0);
    int x1= get_value_offset_sp(hashMap,value1);
    if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
        int x=value1->pdata->var_pdata.iVal;
        printf("    mov r0,#%d\n",x);
        printf("    str r0,[sp,#%d]\n",x0);
        return ins;
    }
    if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
        int x=value1->pdata->var_pdata.iVal;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",x);
        printf("    ldr r1,=%s\n",arr1);
        printf("    str r0,[sp,#%d]\n",x0);
        return ins;
    }
    if(isImmFloatType(value1->VTy)){
        return ins;
        ;
    }
    if(isLocalVarIntType(value1->VTy)){
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            printf("    ldr r0,[sp,#%d]\n",x1);
            printf("    str r0,[sp,#%d]\n",x0);
        } else{
            ;
        }
        return ins;
    }
    if(isLocalVarFloatType(value1->VTy)){
        printf("    vldr s0,[sp,#%d]\n",x1);
        printf("    vstr s0,[sp,#%d]\n",x0);
        return ins;
    }
//    两个unknown的情况
    offset *node= HashMapGet(hashMap,value1);
    if(node->memory){
        printf("    ldr r0,[sp,#%d]\n",x1);
        printf("    str r0,[sp,#%d]\n",x0);
    }

//    printf("CopyOperation\n");
    return ins;
}
InstNode * arm_trans_Add(InstNode *ins,HashMap*hashMap){

    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    add r0,#%d,#%d\n",x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    add r0,r1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    add r0,#%d,r2\n",x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr1);
            printf("    ldr r2,=%s\n",arr2);
            printf("    add r0,r1,r2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");
            printf("    fadd s0,s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");

            printf("    fadd s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fadd s0,s1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fadd s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");

        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    add r0,#%d,r2\n",x1);
            }else{
                int x=node->regr;
                printf("    add r0,#%d,r%d\n",x1,x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    add r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    add r0,r1,r%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fadd s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fadd s0,s1,s%d\n",x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fadd s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fadd s0,s1,s%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s2.r2\n");
        }else{
            int x=node->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s2,s%d\n",x);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    add r0,r1,#%d\n",x2);
            }else{
                int x=node->regr;
                printf("    add r0,r%d,#%d\n",x,x2);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    add r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    add r0,r%d,r2\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s1,r1\n");
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fadd s0,s1,s2\n");
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fadd s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r1,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r1,r%d",x1);
            printf("    mov r2,r%d\n",x2);
        }
        printf("    add r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fadd s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    return  ins;
}

InstNode * arm_trans_Sub(InstNode *ins,HashMap*hashMap){

    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    sub r0,#%d,#%d\n",x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    sub r0,r1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    sub r0,#%d,r2\n",x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr1);
            printf("    ldr r2,=%s\n",arr2);
            printf("    sub r0,r1,r2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");
            printf("    fsub s0,s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");

            printf("    fsub s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fsub s0,s1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fsub s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");

        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    sub r0,#%d,r2\n",x1);
            }else{
                int x=node->regr;
                printf("    sub r0,#%d,r%d\n",x1,x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    sub r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    sub r0,r1,r%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fsub s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fsub s0,s1,s%d\n",x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fsub s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fsub s0,s1,s%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s2.r2\n");
        }else{
            int x=node->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s2,s%d\n",x);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    sub r0,r1,#%d\n",x2);
            }else{
                int x=node->regr;
                printf("    sub r0,r%d,#%d\n",x,x2);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    sub r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    sub r0,r%d,r2\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s1,r1\n");
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fsub s0,s1,s2\n");
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fsub s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r1,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r1,r%d",x1);
            printf("    mov r2,r%d\n",x2);
        }
        printf("    sub r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fsub s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    return  ins;

}

InstNode * arm_trans_Mul(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    mov r1,#%d\n",x1);
            printf("    mov r2,#%d\n",x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    mov r2,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("    mov r1,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr1);
            printf("    ldr r2,=%s\n",arr2);
        }
        printf("    mul r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");
            printf("    fmul s0,s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");

            printf("    fmul s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fmul s0,s1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fmul s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");

        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    mov r1,#%d\n",x1);
                printf("    mul r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mov r1,#%d\n",x1);
                printf("    mov r2,r%d\n",x);
                printf("    mul r0,r1,r2\n",x1);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    mul r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mul r0,r1,r%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fmul s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fmul s0,s1,s%d\n",x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fmul s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fmul s0,s1,s%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s2.r2\n");
        }else{
            int x=node->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s2,s%d\n",x);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    mov r2,#%d\n",x2);
                printf("    mul r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mov r2,#%d\n",x2);
                printf("    mul r0,r%d,r2\n",x);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    mul r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mul r0,r%d,r2\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s1,r1\n");
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fmul s0,s1,s2\n");
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fmul s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r1,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r1,r%d",x1);
            printf("    mov r2,r%d\n",x2);
        }
        printf("    mul r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fmul s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    return  ins;
}

InstNode * arm_trans_Div(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    mov r1,#%d\n",x1);
            printf("    mov r2,#%d\n",x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    mov r2,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("    mov r1,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr1);
            printf("    ldr r2,=%s\n",arr2);
        }
        printf("    sdiv r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");
            printf("    fdiv s0,s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");

            printf("    fdiv s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fdiv s0,s1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fdiv s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");

        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    mov r1,#%d\n",x1);
                printf("    sdiv r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mov r1,#%d\n",x1);
                printf("    mov r2,r%d\n",x);
                printf("    sdiv r0,r1,r2\n",x1);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
                printf("    sdiv r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    sdiv r0,r1,r%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fdiv s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fdiv s0,s1,s%d\n",x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
                printf("    fdiv s0,s1,s2\n");
            }else{
                int x= node->regs;
                printf("    fdiv s0,s1,s%d\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s2.r2\n");
        }else{
            int x=node->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s2,s%d\n",x);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    mov r2,#%d\n",x2);
                printf("    sdiv r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    mov r2,#%d\n",x2);
                printf("    sdiv r0,r%d,r2\n",x);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    sdiv r0,r1,r2\n");
            }else{
                int x=node->regr;
                printf("    sdiv r0,r%d,r2\n",x);
            }
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x);
            printf("    fcvt.s32.f32 s1,r1\n");
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fdiv s0,s1,s2\n");
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory) {
                int x = get_value_offset_sp(hashMap, value1);
                printf("    vldr s1,[sp,#%d]\n", x);
            } else{
                int x=node->regs;
                printf("    fmov s1,s%d\n",x);
            }
            printf("    fdiv s0,s1,s2\n");
        }
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r1,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r1,r%d",x1);
            printf("    mov r2,r%d\n",x2);
        }
        printf("    sdiv r0,r1,r2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r0\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r0\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fdiv s0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)||isLocalArrayIntType(value0->VTy)||isGlobalVarIntType(value0->VTy)||isGlobalArrayIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 r0,s0\n");
                printf("    str r0,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    fcvt.s32.f32 r%d,s0\n",x);
            }
        }
        else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s%d,s0\n",x);
            }
        }
    }
    return  ins;
}

InstNode * arm_trans_Module(InstNode *ins,HashMap*hashMap){
//    这个比较简单，只会存在两种情况，就是int=int1 % int2和float=int1 % int2,右边出现非int都是错误的
// 1 ****************************************
// 将int1放在r0，将int2放在r1，然后执行bl	__aeabi_idivmod(PLT)
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&& isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    mov r0,#%d\n",x1);
            printf("    mov r1,#%d\n",x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r0,=%s\n",arr1);
            printf("    mov r1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("    mov r0,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr2);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r0,=%s\n",arr1);
            printf("    ldr r1,=%s\n",arr2);
        }
        printf("    bl __aeabi_idivmod(PLT)\n");
        if(isLocalVarIntType(value0->VTy)|| isGlobalVarIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r1,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r1\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r1\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r1\n",x);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r1,[sp,#%d]\n",x);
                printf("    mov r0,#%d\n",x1);
            }else{
                int x=node->regr;
                printf("    mov r0,#%d\n",x1);
                printf("    mov r1,r%d\n",x);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r0,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r1,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r1,r%d\n",x);
            }
        }
        printf("    bl __aeabi_idivmod(PLT)\n");
        if(isLocalVarIntType(value0->VTy)|| isGlobalVarIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r1,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r1\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r1\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r1\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r0,[sp,#%d]\n",x);
                printf("    mov r1,#%d\n",x2);
            }else{
                int x=node->regr;
                printf("    mov r0,r%d\n",x);
                printf("    mov r1,#%d\n",x2);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r0,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r0,r%d\n",x);
            }
        }
        printf("    bl __aeabi_idivmod(PLT)\n");
        if(isLocalVarIntType(value0->VTy)|| isGlobalVarIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r1,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r1\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r1\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r1\n",x);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r0,[sp,#%d]\n",x1);
            printf("    ldr r1,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r0,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r1,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r0,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r0,r%d",x1);
            printf("    mov r1,r%d\n",x2);
        }
        printf("    bl __aeabi_idivmod(PLT)\n");
        if(isLocalVarIntType(value0->VTy)|| isGlobalVarIntType(value0->VTy)){
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    str r1,[sp,#%d]\n",x);
            } else{
                int x=node->regr;
                printf("    mov r%d,r1\n",x);
            }
        } else{
            offset *node= HashMapGet(hashMap,value0);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value0);
                printf("    fcvt.s32.f32 s0,r1\n");
                printf("    vstr s0,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fcvt.s32.f32 s%d,r1\n",x);
            }
        }
    }

//    printf("arm_trans_Module\n");
    return ins;
}

InstNode * arm_trans_Call(InstNode *ins){
//    printf("CALL\n");
    printf("    bl %s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    return ins;
}

InstNode * arm_trans_FunBegin(InstNode *ins,int *stakc_size){
    printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    InstNode *tmp=ins;
//        这里好像是如果main中调用了其他的函数，就需要将r11,sp压栈,所以说需要查找一下有没有call
//        这个好像所有函数都是一样的，如果它调用了其他的函数的话，就需要保存r11(也称为fp)和sp，那这样的话
//        main函数好像和其他的函数就没有什么区别了
//    if(!strcmp("main", user_get_operand_use(&ins->inst->user,0)->Val->name)){
//        printf("    push {r11,lr}\n");
////        printf("    mov r1,sp\n");
//    }
    for(;tmp!=NULL && tmp->inst->Opcode!= Return;tmp= get_next_inst(tmp)){
        if(tmp->inst->Opcode==Call){
            func_call_func+=1;
            break;
        }
    }
    if(func_call_func>0){
        printf("    stmfd sp!,{r11,sp}\n");
    }

    HashMap *hashMap=HashMapInit();
//    HashMapSetHash(hashMap,HashKey);
//    HashMapSetCompare(hashMap,CompareKey);
//    HashMapSetCleanKey(hashMap,CleanKey);
//    HashMapSetCleanValue(hashMap,CleanValue);


    int local_stack=0;
//    int x= get_siezof_sp(hashMap);
//    printf("    sub sp,sp,#%d\n",4*x);
    //    在函数开始的时候要进行参数传递的str的处理
    int param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
    char name[20];
    sprintf(name, "%c", '%');
    sprintf(name + 1, "%d", param_num);
    InstNode *temp=ins;
    for (; ins != NULL && ins->inst->Opcode != Return; ins = get_next_inst(ins)) {
        Value *value0, *value1, *value2;
        switch (ins->inst->Opcode) {
            case Alloca:
                value0 = &ins->inst->user.value;
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_alloca_add(hashMap,value0,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case Add:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Sub:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Mul:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Div:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Module:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Call:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case Store:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case Load:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case GIVE_PARAM:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case LESS:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case GREAT:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case LESSEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case GREATEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case EQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case NOTEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
            case XOR:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case zext:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case bitcast:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case GEP:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
//            case MEMCPY:
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
//                break;
            case GLOBAL_VAR:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                break;
//            case MEMSET:
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
//                break;
            case CopyOperation:
                value0=ins->inst->user.value.alias;
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            default:
                break;
        }

    }
//    *stakc_size=mystack+param_num*4;
//    int x= HashMapSize(hashMap)*4;
//    printf("FuncBeginhashmapsize=%d\n",x+param_num*4);
    *stakc_size=local_stack+param_num*4;
    printf("    sub sp,sp,#%d\n",*stakc_size);
    if(param_num>0){
//        存在参数的传递
//        int local_stack=mystack;
        for(int j=0;j<param_num && j<4;j++){
            printf("    str r%d,[sp,#%d]\n",j,local_stack+j*4);
        }
    } else{
        ;
    }

//    printf("localstack=%d\n",local_stack);
//    int myx= HashMapSize(hashMap)*4;
//    printf("hashmapsize=%d\n",myx);

    HashMapDeinit(hashMap);

    ins=temp;


    return ins;
}

InstNode * arm_trans_Return(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size){
//涉及到数组的时候，栈帧的大小并不等于HashMapSize(hashmap)*4
//    if(!strcmp("main", user_get_operand_use(&head->inst->user,0)->Val->name)){
//        int x1= stack_size;
//
//        printf("    add sp,sp,#%d\n",x1);
////        printf("    mov r11,sp\n");
//        if(func_call_func>0){
//            printf("    pop {r11,lr}\n");
//        }
//        printf("    bx lr\n");
//        return ins;
//    }

    int x1= stack_size;
    printf("    add sp,sp,#%d\n",x1);
    if(func_call_func>0){
        printf("    ldmfd sp!,{r11,lr}\n");
    }
    printf("    bx lr\n");
    return ins;
}

InstNode * arm_trans_Alloca(InstNode *ins,HashMap*hashMap){
//    在汇编中，alloca不需要翻译,但是栈帧分配的时候需要用到。
// 这个现在暂定是用来进行数组的初始话操作。bl memset来进行处理，就不需要调用多次store
    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    if(isLocalArrayIntType(value0->VTy)|| isLocalArrayFloatType(value0->VTy)|| isGlobalArrayIntType(value0->VTy)||isGlobalArrayFloatType(value0->VTy)){
//        printf("%s\n",value0->alias->name);
        int x=get_value_offset_sp(hashMap,value0);
        printf("    add r0,sp,#%d\n",x);
        printf("    mov r1,#0\n");
        x= get_array_total_occupy(value0->alias,0);
        printf("    mov r2,#%d\n",x);
        printf("    bl memset\n");
    }
    return ins;
}

InstNode * arm_trans_GIVE_PARAM(InstNode *ins,HashMap*hashMap){
//  这个是用来标定参数传递的，这个可不仅仅是一个标定作用，
//  这个是在处理数组传参（传数组首地址），和地址指针的时候需要用到。

//要考虑吧普通的参数传递和数组地址参数传递，还要考虑参数个数的问题。如果传递的是float型的参数呢？使用IEEE754放在通用寄存器中
//    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    计算参数的传递个数
    int num=0;
    InstNode *tmp=ins;
    for(;tmp->inst->Opcode==GIVE_PARAM;tmp= get_next_inst(tmp)){
        num++;
    }
    tmp=ins;
    if(num<=4){
        for(int i=0;i<num;i++){
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            int x= get_value_offset_sp(hashMap,value1);
            if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递
                printf("    add r%d,sp,#%d\n",i,x);

            } else{
                printf("    ldr r%d,[sp,#%d]\n",i,x);
            }

//            直接在这个地方判断类型，然后加上add ri,sp,#%d好像就可以了

            tmp= get_next_inst(tmp);
        }
    }else{
        tmp= get_next_inst(tmp);
        for(int i=1;i<4;++i){
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            int x= get_value_offset_sp(hashMap,value1);
            if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递
                printf("    add r%d,sp,#%d\n",i,x);


            } else{
                printf("    ldr r%d,[sp,#%d]\n",i,x);
            }
            tmp= get_next_inst(tmp);
        }
        for (int i = 1; i <=num-4; ++i) {
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            int x= get_value_offset_sp(hashMap,value1);
            if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递
                printf("    add r0,sp,#%d\n",i,x);

            } else{
//                printf("    ldr r%d,[sp,#%d]\n",i,x);
                printf("    ldr r0,[sp,#%d]\n",x);
                printf("    str r0,[sp,#-%d]\n",i*4);
            }
//            printf("    ldr r0,[sp,#%d]\n",x);
//            printf("    str r0,[sp,#-%d]\n",i*4);
            tmp= get_next_inst(tmp);
        }
        tmp=ins;
        Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
        int x= get_value_offset_sp(hashMap,value1);
        printf("    ldr r0,[sp,#%d]\n",x);
    }
    tmp=ins;
    while (get_next_inst(tmp)->inst->Opcode==GIVE_PARAM){
        tmp= get_next_inst(tmp);
    }
    ins=tmp;
//    int x=get_value_offset_sp(hashMap,value1);
//    printf("hello world!\n")
    return ins;
}

InstNode * arm_trans_ALLBEGIN(InstNode *ins){
//    int i=ins->inst->i;
    printf("**********ALLBEGIN**************\n");
    return ins;
}

InstNode * arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(InstNode *ins,HashMap*hashMap){

    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("    cmp #%d,#%d\n",x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr[12]="0x";
            sprintf(arr+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr);
            printf("    cmp r1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr[12]="0x";
            sprintf(arr+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr);
            printf("    cmp #%d,r2\n",x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r1,=%s\n",arr1);
            printf("    ldr r2,=%s\n",arr2);
            printf("    cmp r1,r2\n");
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");
            printf("    fcmp s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fmov s2,r2\n");

            printf("    fcmp s1,s2\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fcmp s0,s1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fmov s1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");

            printf("    fcmp s1,s2\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fmov s1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");

        printf("    fcmp s1,s2\n");
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r2,r%d\n",x);
            }
            printf("    cmp #%d,r2\n",x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    ldr r2,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r2,r%d\n",x);
            }
            printf("    cmp r1,r2\n");
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("    mov r1,#%d\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s2,s%d\n",x);
            }
            printf("    fcmp s1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("    ldr r1,=%s\n",arr1);
            printf("    fcvt.s32.f32 s1,r1\n");
            offset *node= HashMapGet(hashMap,value2);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value2);
                printf("    vldr s2,[sp,#%d]\n",x);
            }else{
                int x=node->regs;
                printf("    fmov s2,s%d\n",x);
            }
            printf("    fcmp s1,s2\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fcvt.s32.f32 s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x);
        }else{
            int x=node->regr;
            printf("    mov r2,r%d\n",x);
        }
        printf("    fcvt.s32.f32 s2,r2\n");
        printf("    fcmp s1,s2\n");
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("    ldr r1,=%s\n",arr1);
        printf("    fcvt.s32.f32 s1,r1\n");
        offset *node= HashMapGet(hashMap,value2);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s2,s%d\n",x);
        }
        printf("    fcmp s1,s0\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r1,r%d\n",x);
            }
            printf("    cmp r1,#%d\n",x2);
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            offset *node= HashMapGet(hashMap,value1);
            if(node->memory){
                int x= get_value_offset_sp(hashMap,value1);
                printf("    ldr r1,[sp,#%d]\n",x);
            }else{
                int x=node->regr;
                printf("    mov r1,r%d\n",x);
            }
            printf("    cmp r1,r2\n");
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fcvt.s32.f32 s2,r2\n");
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x);
        }else{
            int x=node->regr;
            printf("    mov r1,r%d\n",x);
        }
        printf("    fcvt.s32.f32 s1,r1\n");
        printf("    fcmp s1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        if(imm_is_valid(x2)){
            printf("    mov r2,#%d\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("    ldr r2,=%s\n",arr2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }
        printf("    fcmp s1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        offset *node= HashMapGet(hashMap,value1);
        if(node->memory){
            int x= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x);
        }else{
            int x=node->regs;
            printf("    fmov s1,s%d\n",x);
        }
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("    ldr r2,=%s\n",arr2);
        printf("    fmov s2,r2\n");
        printf("    fcmp s1,s2\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    mov r2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            int x1=node1->regr;
            printf("    mov r1,r%d\n",x1);
        }else{
            int x1=node1->regr;
            int x2=node2->regr;
            printf("    mov r1,r%d",x1);
            printf("    mov r2,r%d\n",x2);
        }
        printf("    cmp r1,r2\n");
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    ldr r1,[sp,#%d]\n",x1);
            printf("    fcvt.s32.f32 s1,r1\n");
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regr;
            printf("    fcvt.s32.f32 s1,r%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fcmp s1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    ldr r2,[sp,#%d]\n",x2);
            printf("    fcvt.s32.f32 s2,r2\n");
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regr;
            printf("    fcvt.s32.f32 s2,r%d\n",x2);
        }
        printf("    fcmp s1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        offset *node1= HashMapGet(hashMap,value1);
        offset *node2= HashMapGet(hashMap,value2);
        if(node1->memory&&node2->memory){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s1,[sp,#%d]\n",x1);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else if(node1->memory&&(!node2->memory)){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("    vldr s1,[sp,#%d]\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }else if((!node1->memory)&&node2->memory){
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("    vldr s2,[sp,#%d]\n",x2);
        }else{
            int x1=node1->regs;
            printf("    fmov s1,s%d\n",x1);
            int x2=node2->regs;
            printf("    fmov s2,s%d\n",x2);
        }
        printf("    fcmp s1,s2\n");
    }

    if(ins->inst->Opcode==LESS){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    bge %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    } else if(ins->inst->Opcode==GREAT){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    ble %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    } else if(ins->inst->Opcode==LESSEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    bgt %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    } else if(ins->inst->Opcode==GREATEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    blt %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    } else if(ins->inst->Opcode==EQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    bne %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    } else if(ins->inst->Opcode==NOTEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("    beq %d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("    b %d\n",x);
        }
    }
    return ins;


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
//类型转换，已经通过映射解决掉了bitcast产生的 多余的mov和load指令的问题
//    printf("arm_trans_bitcast\n");
    return ins;
}

InstNode * arm_trans_GMP(InstNode *ins,HashMap*hashMap){
// 数组初始化
// 获取栈帧首地址使用的是%i，不需要进到alias中
    Value *array= user_get_operand_use(&ins->inst->user,0)->Val;

    for(;get_next_inst(ins)->inst->Opcode == GEP;){
        ins= get_next_inst(ins);
    }
//    这里得到最后一条GMP指令
    int off_sp= get_value_offset_sp(hashMap,array)+ins->inst->user.value.pdata->var_pdata.iVal*4;
//    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    if(get_next_inst((ins))->inst->Opcode==Store){
        InstNode *next= get_next_inst(ins);
//        int off= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
        int x= user_get_operand_use(&next->inst->user,0)->Val->pdata->var_pdata.iVal;
        printf("    mov r0,#%d\n",x);
//        int off_sp= get_value_offset_sp(hashMap, user_get_operand_use(&next->inst->user,1)->Val->alias)+off;
        printf("    str r0,[sp,#%d]\n",off_sp);
        return next;
    }
    if(get_next_inst((ins))->inst->Opcode==Load){
//        如果是load的话，我需要考虑将对应的值load到哪个寄存器
        InstNode *next= get_next_inst(ins);
        int x= get_value_offset_sp(hashMap,&next->inst->user.value);
        printf("    ldr r1,[sp,#%d]\n",off_sp);
        printf("    str r1,[sp,#%d]\n",x);
        return next;
    }

//*************************************************************************
//这里是在数组处理的store指令没有被删除的时候的处理方式,理论上来说
//    if(get_next_inst((ins))->inst->Opcode==Store){
//        InstNode *next= get_next_inst(ins);
//        int off= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
//        int x= user_get_operand_use(&next->inst->user,0)->Val->pdata->var_pdata.iVal;
//        printf("    mov r0,#%d\n",x);
//        int off_sp= get_value_offset_sp(hashMap, user_get_operand_use(&next->inst->user,1)->Val->alias)+off;
//        printf("    store r0,[sp,#%d]\n",off_sp*4);
//        return next;
//    }



//    printf("\n");
    return ins;
}

InstNode * arm_trans_MEMCPY(InstNode *ins){

//    涉及到数组的内容,后端不需要翻译这条ir和memcpy对应的ir
//    printf("arm_trans_MEMCPY\n");
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

//    printf("arm_trans_Phi\n");
    return ins;
}

InstNode *arm_trans_MEMSET(InstNode *ins){

//    printf("arm_trans_MEMSET\n");
    return ins;
}

InstNode *_arm_translate_ins(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size){

    int x=ins->inst->Opcode;
    switch(x){
        case Add:
            return arm_trans_Add(ins,hashMap);
        case Sub:
            return arm_trans_Sub(ins,hashMap);
        case Mul:
            return arm_trans_Mul(ins,hashMap);
        case Div:
            return arm_trans_Div(ins,hashMap);
        case Module:
            return arm_trans_Module(ins,hashMap);
        case Call:
//            在进行call之前是需要至少保存lr寄存器的，call调用结束之后还需要将lr出栈恢复
            return arm_trans_Call(ins);
//        case FunBegin:
//            return arm_trans_FunBegin(ins,hashMap);
        case Return:
            return arm_trans_Return(ins,head,hashMap,stack_size);
        case Store:
//            return arm_trans_Store(ins,hashMap);

            printf("    store\n");
            return ins;
        case Load:
//            return arm_trans_Load(ins,hashMap);
            printf("    load\n");
            return ins;
        case Alloca:
            return arm_trans_Alloca(ins,hashMap);
        case GIVE_PARAM:
            return arm_trans_GIVE_PARAM(ins,hashMap);
        case ALLBEGIN:
            return arm_trans_ALLBEGIN(ins);
        case LESS:
        case GREAT:
        case LESSEQ:
        case GREATEQ:
        case EQ:
        case NOTEQ:
            return arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(ins,hashMap);
        case br_i1:
            return arm_trans_br_i1(ins);
        case br:
            return arm_trans_br(ins);
        case br_i1_true:
            return arm_trans_br_i1_true(ins);
        case br_i1_false:
            return arm_trans_br_i1_false(ins);
        case Label:
            return arm_trans_Label(ins);
        case tmp:
            return arm_trans_tmp(ins);
        case XOR:
            return arm_trans_XOR(ins);
        case zext:
            return arm_trans_zext(ins);
        case bitcast:
            return arm_trans_bitcast(ins);
        case GEP:
            return arm_trans_GMP(ins,hashMap);
        case MEMCPY:
            return arm_trans_MEMCPY(ins);
        case zeroinitializer:
            return arm_trans_zeroinitializer(ins);
        case GLOBAL_VAR:
            return arm_trans_GLOBAL_VAR(ins);
        case Phi:
            return arm_trans_Phi(ins);
        case MEMSET:
            return arm_trans_MEMSET(ins);
        case CopyOperation:
            return arm_trans_CopyOperation(ins,hashMap);
        default:
            return ins;
    }
}
