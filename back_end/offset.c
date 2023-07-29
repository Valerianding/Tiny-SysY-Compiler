//
// Created by ljf on 23-2-25.
//

#include "offset.h"



HashMap *global_hashmap;
int func_num=0;
int in_func_num=0;

offset *offset_node(){
    offset *node=(offset*) malloc(sizeof(offset));
    memset(node,0, sizeof(offset));
    return node;
}
//void globalint_mapping(Value*value0,Value*value1){
//
//}
void offset_free(HashMap*hashMap){
//这个会出现double free的问题，具体在哪里出现的还需要找一下
//    HashMapFirst(hashMap);
//    Pair *ptr_pair;
//    while ((ptr_pair= HashMapNext(hashMap))!=NULL){
//        offset *node=(offset*)ptr_pair->value;
//        if(node!=NULL){
//            free(node);
//        }
//        node=NULL;
//    }
    HashMapDeinit(hashMap);
    return;
}

void hashmap_add(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int reg_save_num,int reg_flag,HashMap*lineScan_param){
//    reg_flag=-1;
//    全局变量放在全局变量的global_hashmap里面，因为已经改为了使用 #:upper:16 和 #:lower:16,所以说下面两个保存的信息已经没有用了
//    if(isGlobalVarFloatType(key->VTy)){
//
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            lcptLabel->INTTRUE__FLOATFALSE=false;
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//
//    }else if(isGlobalVarIntType(key->VTy)){
//
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            lcptLabel->INTTRUE__FLOATFALSE=true;
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//
//    }
////    现在需要将全局变量的数组的处理也要加上,全局变量数组的使用和普通全局变量的使用差不多
////    也是.LCPI_0_0:
////	            .long	brr
////	  这样的形式来进行处理的
////    加进去之后，具体的处理方式就是放在数组的处理里里面了，需要处理全局int型，全局float型，GMP和load和store里面去进行处理了
//    else if(isGlobalArrayIntType(key->VTy)){
//
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            lcptLabel->INTTRUE__FLOATFALSE=true;
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//
//    } else if(isGlobalArrayFloatType(key->VTy)){
//
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            lcptLabel->INTTRUE__FLOATFALSE=false;
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//
//    }

//    局部变量就是直接放在hashmap里面

    if((!isImmIntType(key->VTy))&&(!isImmFloatType(key->VTy)) ){
        if(!HashMapContain(hashMap,key)){
//            printf("name:%s  keyname:%s\n",name,key->name);
            int name_num= atoi(name+1);
            int key_name_num=-1;
            if(key->name!=NULL && key->name[0]=='%'){ //不加key->name[0]=='%'这个，在判断全局变量@n的时候会出错
                key_name_num= atoi(key->name+1);
            }
//            if(strcmp(key->name,name)<0 &&  strlen(key->name) <= strlen(name)){
            if(key_name_num!=-1 && key_name_num<name_num){
//                按照之前的处理是，传递过来的参数都是放在栈的顶部来进行存放的，
//                这里生成的仅仅是供给获取偏移量的时候使用，是一说这里计算其于sp距离的时候，
//                还需要考虑其是前四个参数还是后面的参数，前四个参数直接将(*add_sp)+=4就可以了，
//                是前后面的参数的话，需要将local_var_num*4+保护寄存器的个数*4+(ri-4)*4如果
//                    表示该参数为传递过来的参数,数组是不需要特别的处理的
                int ri= atoi((key->name)+1);
                offset *temp=offset_node();
                if(ri<=3&&ri>=0){
//                    当普通局部变量来映射
                    temp->offset_sp=(*add_sp);
//                    printf("str %s,%d\n",key->name,temp->offset_sp);
                    (*add_sp)+=4;
                }else{
//                    最顶部的参数变量来进行映射,被保护的寄存器的变量个数需要传过来
                    temp->offset_sp=(*local_var_num)*4+(reg_save_num*4)+(ri-4)*4;
//                    printf("str %s,%d\n",key->name,temp->offset_sp);
                }
//                offset *temp=offset_node();
//                printf("sub_sp=%d\n",*sub_sp);
//                temp->offset_sp=(*local_var_num)*4+ri*4;
//                这里还要处理unknow的情况,默认为int
                if(isLocalVarFloatType(key->VTy)|| isLocalArrayFloatType(key->VTy)){
                    temp->INTTRUE__FLOATFALSE=false;
                } else{
                    temp->INTTRUE__FLOATFALSE=true;
                }
                temp->memory=true;
                temp->regr=-1;
                temp->regs=-1;
//                printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
                HashMapPut(hashMap,key,temp);
                HashMapPut(lineScan_param,key,temp);
            }else if(reg_flag<0){ //寄存器为负数表示需要回存内存
//                if(isLocalArrayIntType(key->VTy)||isLocalArrayFloatType(key->VTy)||isGlobalArrayIntType(key->VTy)||isGlobalArrayFloatType(key->VTy)){
////                    处理数组
//                    int size_array= get_array_total_occupy(key->alias,0);
//                    offset *temp=offset_node();
//                    temp->offset_sp=(*add_sp);
//                    (*add_sp)+=size_array;
//                    temp->memory=true;
//                    temp->regr=-1;
//                    temp->regs=-1;
////                    printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
//                    HashMapPut(hashMap,key,temp);
//                }else
//                {
                offset *temp=(offset*) malloc(sizeof(offset));
                temp->offset_sp=(*add_sp);
//                printf("str %s,%d\n",key->name,temp->offset_sp);
//                printf("add_sp=%d\n",*add_sp);
                (*add_sp)+=4;
                if(isLocalVarFloatType(key->VTy)|| isLocalArrayFloatType(key->VTy)){
                    temp->INTTRUE__FLOATFALSE=false;
                } else{
                    temp->INTTRUE__FLOATFALSE=true;
                }
                temp->memory=true;
                temp->regr=-1;
                temp->regs=-1;
//                    printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
                HashMapPut(hashMap,key,temp);
//                }
            }
        }
    }

//    else{
////        %i为立即数
//        if(!HashMapContain(hashMap,key)){
////            printf("haha\n");
//            offset *node=offset_node();
//            node->offset_sp=(*add_sp);
//            (*add_sp)+=4;
//            node->memory=true;
//            node->regs=-1;
//            node->regr=-1;
////            printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
//            HashMapPut(hashMap,key,node);
//        }
//    }

    return;
}


//void hashmap_add_left(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int regri){
////    全局变量放在全局变量的global_hashmap里面
//    if(isGlobalVarFloatType(key->VTy)){
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//    }else if(isGlobalVarIntType(key->VTy)){
//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }
//    }else if(isGlobalArrayIntType(key->VTy)){
//        ;
//    } else if(isGlobalArrayIntType(key->VTy)){
//        ;
//    }
////    局部变量就是直接放在hashmap里面
//    else if((!isImmIntType(key->VTy))&&(!isImmFloatType(key->VTy)) ){
//        if(!HashMapContain(hashMap,key)){
//            if(strcmp(key->name,name)<0 &&  strlen(key->name) <= strlen(name)){
////                    表示该参数为传递过来的参数,数组是不需要特别的处理的
//                int ri= atoi((key->name)+1);
//                offset *temp=offset_node();
//                temp->offset_sp=(*local_var_num)*4+ri*4;
//                temp->memory=true;
//                temp->regr=-1;
//                temp->regs=-1;
//                HashMapPut(hashMap,key,temp);
//            }else{
//                offset *temp=(offset*) malloc(sizeof(offset));
//                temp->offset_sp=(*add_sp);
//                (*add_sp)+=4;
//                temp->memory=true;
//                temp->regr=-1;
//                temp->regs=-1;
//                HashMapPut(hashMap,key,temp);
//            }
//        }
//    }
//
//    return;
//}
//void hashmap_add_right(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num,int regri);
void hashmap_alloca_add(HashMap*hashMap,Value*key,int *add_sp,int reg_save_num){
    if(!HashMapContain(hashMap,key)){
        if(isLocalArrayIntType(key->VTy)||isLocalArrayFloatType(key->VTy)||isGlobalArrayIntType(key->VTy)||isGlobalArrayFloatType(key->VTy)){
            int size_array= get_array_total_occupy(key->alias,0);
            offset *node=offset_node();
            node->offset_sp=(*add_sp);
            (*add_sp)+=size_array;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,key,node);
        } else{
            assert(false);
            offset *node=offset_node();
            node->offset_sp=(*add_sp);
            (*add_sp)+=4;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,key,node);
        }
    }
    return;
}
void hashmap_bitcast_add(HashMap*hashMap,Value*key,Value *value,int reg_save_num){
//    printf("%p\n",key);
    if(!HashMapContain(hashMap,key)){
        if(HashMapContain(hashMap,value)){
            offset *tmp= HashMapGet(hashMap,value);
            offset *node= offset_node();
            node->offset_sp=tmp->offset_sp;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,key,node);
        }else{
            assert(false);
        }
    }
    return;
}

HashMap *offset_init(InstNode*ins,int *local_var_num,int reg_save_num,HashMap *lineScan_param){
//    printf("local var num=%d\n",*local_var_num);
    HashMap *hashMap=HashMapInit();
    int sub_sp=0;
    int add_sp=0;
    char name[20];
    int param_num= user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
    sprintf(name,"%c",'%');
    sprintf(name+1,"%d",param_num);
//    printf("name:%s\n",name);
//    这个在hashmap_add函数里面进行了处理
    for(;ins!=NULL&&ins->inst->Opcode!=Return;ins= get_next_inst(ins)){
        Value *value0,*value1,*value2;
        Value *value0_alias;
        int operandNum;
        switch (ins->inst->Opcode) {
//            case Alloca:
//                用来处理数组何全局变量,数组可以不通过这个来获取处理相关的值


//                这个需要补充
//                Value * value0_alias=&ins->inst->user.value.alias;
//                value0_alias=ins->inst->user.value.alias;
////                这个是用来处理掉MAIN_INT和其他一些alloca指令的干扰的.
//                if(value0_alias==NULL){
//                    hashmap_add(hashMap,&ins->inst->user.value,name,&sub_sp,&add_sp,local_var_num);
//                }else{
//                    hashmap_add(hashMap,value0_alias,name,&sub_sp,&add_sp,local_var_num);
//                }
//                break;
            case Alloca:
                value0 = &ins->inst->user.value;
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                hashmap_alloca_add(hashMap,value0,&add_sp,reg_save_num);
                break;
            case Add:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Sub:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Mul:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Div:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Mod:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Call:
//                operandNum=ins->inst->user.value.NumUserOperands;
//                if(operandNum==2){
                  if(returnValueNotUsed(ins)){
                      value1=user_get_operand_use(&ins->inst->user,0)->Val;
                      hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                  }else{
                      value0=&ins->inst->user.value;
                      value1=user_get_operand_use(&ins->inst->user,0)->Val;
//                    if(value0->VTy!=Unknown){
////                    ==unknow说明回值为void
//                        hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0]);
//                    }
                      hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                      hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                  }

//                }else if(operandNum==1){
//                    value1=user_get_operand_use(&ins->inst->user,0)->Val;
//                    hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1]);
//                }
                break;
            case Store:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case Load:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);

                break;
            case GIVE_PARAM:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                break;
            case LESS:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case GREAT:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case LESSEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case GREATEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case EQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case NOTEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
            case XOR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                break;
            case zext:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                break;
            case bitcast:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_bitcast_add(hashMap,value0,value1,reg_save_num);
//                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case GEP:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
//                hashmap_bitcast_add(hashMap,value0,value1,reg_save_num);
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;
//            case MEMCPY:
//                value1=user_get_operand_use(&ins->inst->user,0)->Val;
//                value2= user_get_operand_use(&ins->inst->user,1)->Val;
//                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
//                break;
            case GLOBAL_VAR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[2],lineScan_param);
                break;

//            case MEMSET:
//                value1=user_get_operand_use(&ins->inst->user,0)->Val;
//                value2= user_get_operand_use(&ins->inst->user,1)->Val;
//                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
//                break;
            case CopyOperation:
                value0=ins->inst->user.value.alias;//这里是需要进到alias里面的
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[0],lineScan_param);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
                break;
            default:
                break;
        }
    }

    if(ins->inst->Opcode==Return){
        int operandNum=ins->inst->user.value.NumUserOperands;
        if(operandNum!=0){
            Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
            hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num,reg_save_num,ins->inst->_reg_[1],lineScan_param);
        }

    }
    func_num++;
    in_func_num=0;
    return hashMap;
//    if(param_num==0){
////        表示不存在参数传递
//    }else{
////        存在参数传递，需要和%i进行比较
//
//    }
}
unsigned HashKey(void* key)
{
    return HashDjb2((char*)key);
}

int CompareKey(void* lhs, void* rhs)
{
    return strcmp((char*)lhs, (char*)rhs);
}

void CleanKey(void* key)
{
    free(key);
}

void CleanValue(void* value)
{
    free(value);
}

