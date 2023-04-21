//
// Created by tom on 23-2-25.
//

#include "offset.h"
void offset_free(HashMap*hashMap){
    HashMapDeinit(hashMap);
    return;
}
offset *offset_node(){
    offset *node=(offset*) malloc(sizeof(offset));
    memset(node,0, sizeof(offset));
    return node;
}

void hashmap_add(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp,int *local_var_num){
    if((!isImmIntType(key->VTy))&&(!isImmFloatType(key->VTy)) ){
        if(!HashMapContain(hashMap,key)){
//            printf("name:%s  keyname:%s\n",name,key->name);
            if(strcmp(key->name,name)<0 &&  strlen(key->name) <= strlen(name)){
//                    表示该参数为传递过来的参数,数组是不需要特别的处理的
                int ri= atoi((key->name)+1);
                offset *temp=offset_node();
//                printf("sub_sp=%d\n",*sub_sp);
                temp->offset_sp=(*local_var_num)*4+ri*4;
                temp->memory=true;
                temp->regr=-1;
                temp->regs=-1;
//                printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
                HashMapPut(hashMap,key,temp);
            }else{
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
//                printf("add_sp=%d\n",*add_sp);
                (*add_sp)+=4;
                temp->memory=true;
                temp->regr=-1;
                temp->regs=-1;
//                    printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
                HashMapPut(hashMap,key,temp);
//                }
            }
        }
    }
    else{
//        %i为立即数
        if(!HashMapContain(hashMap,key)){
//            printf("haha\n");
            offset *node=offset_node();
            node->offset_sp=(*add_sp);
            (*add_sp)+=4;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
//            printf("haspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,key->name,key);
            HashMapPut(hashMap,key,node);
        }
    }
    return;
}
void hashmap_alloca_add(HashMap*hashMap,Value*key,int *add_sp){
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
void hashmap_bitcast_add(HashMap*hashMap,Value*key,Value *value){
    if(!HashMapContain(hashMap,key)){
        if(HashMapContain(hashMap,value)){
            offset *node= HashMapGet(hashMap,value);
            HashMapPut(hashMap,key,node);
        }
    }
    return;
}
HashMap *offset_init(InstNode*ins,int *local_var_num){
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
                hashmap_alloca_add(hashMap,value0,&add_sp);
                break;
            case Add:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Sub:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Mul:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Div:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Mod:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Call:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                if(value0->VTy!=Unknown){
                    hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                }
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Store:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case Load:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case GIVE_PARAM:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case LESS:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case GREAT:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case LESSEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case GREATEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case EQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case NOTEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
                break;
            case XOR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case zext:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case bitcast:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_bitcast_add(hashMap,value0,value1);
//                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            case GEP:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_bitcast_add(hashMap,value0,value1);
//                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
//                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
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
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp,local_var_num);
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
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp,local_var_num);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
                break;
            default:
                break;
        }
    }
    if(ins->inst->Opcode==Return){
        Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
        hashmap_add(hashMap,value1,name,&sub_sp,&add_sp,local_var_num);
    }
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

