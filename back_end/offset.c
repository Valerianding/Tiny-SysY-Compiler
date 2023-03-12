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
void hashmap_add(HashMap*hashMap,Value*key,char *name,int *sub_sp,int *add_sp){
    if(!isImm(key)){
        if(!HashMapContain(hashMap,key)){
//            printf("name:%s  keyname:%s\n",name,key->name);
            if(strcmp(name,key->name)>0&& (strlen(name)>= strlen(key->name))){
//                    表示该参数为传递过来的参数
                offset *temp=offset_node();
                (*sub_sp)-=4;
//                printf("sub_sp=%d\n",*sub_sp);
                temp->offset_sp=(*sub_sp);
                HashMapPut(hashMap,key,temp);
            }else if(strcmp(name,key->name)<=0){
                offset *temp=(offset*) malloc(sizeof(offset));
                temp->offset_sp=(*add_sp);
//                printf("add_sp=%d\n",*add_sp);
                (*add_sp)+=4;
                HashMapPut(hashMap,key,temp);
            }
        }
    }
    return;
}
HashMap *offset_init(InstNode*ins){
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
        switch (ins->inst->Opcode) {
            case Add:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case Sub:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case Mul:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case Div:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case Call:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case Store:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case Load:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case GIVE_PARAM:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case LESS:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case GREAT:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case LESSEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case GREATEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case EQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case NOTEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case XOR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case zext:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case bitcast:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                break;
            case GMP:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case MEMCPY:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case GLOBAL_VAR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value0,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            case MEMSET:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
                hashmap_add(hashMap,value2,name,&sub_sp,&add_sp);
                break;
            default:
                break;
        }
    }
    if(ins->inst->Opcode==Return){
        Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
        hashmap_add(hashMap,value1,name,&sub_sp,&add_sp);
    }
    return hashMap;
//    if(param_num==0){
////        表示不存在参数传递
//    }else{
////        存在参数传递，需要和%i进行比较
//
//    }
}
