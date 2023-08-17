//
// Created by ljf on 2023/7/27.
//
#include "line_scan.h"

#define S 26
#define R 11
//#define YAOWEI_TEST
int enable_ordinary=1; //通用寄存器分配开关
int enable_globalVar=0;//在启用通用寄存器开关时该开关才有效，控制是否为全局变量地址分配寄存器

int enable_vfp=1; //浮点寄存器分配开关
int flag_lr=1; //释放lr
int flag_r11=1; //释放r11,释放了r11

//还需要一个active(这个可以是PriorityQueue)，和location(这个可以是HashSet)。
PriorityQueue *active;
//最终的寄存器分配方案，location中的成员代表在内存，result中成员代表成功分配寄存器func->lineScanReg
HashSet *location;
//HashMap *result; //key =Value* value=value_register* (int),不需要另外建立，直接使用Function里面的lineScanReg来存放

// 浮点寄存器分配
PriorityQueue *VFPactive;
HashSet *VFPlocation;

int VFPreg[32];
int free_VFPreg_num;
int myreg[16];
int free_reg_num;

//传入的是FuncBegin的ins
void get_function_live_interval(Function*curFunction){
//    BasicBlock *block;
//    Function *function;
//    block=ins->inst->Parent;
//    function=block->Parent;
//    assert(function->ToPoBlocks!=NULL);
//    PriorityQueue *queue=PriorityQueueInit();
//    queue->set_compare(queue,CompareNumerics);

    assert(curFunction!=NULL);
    assert(curFunction->ToPoBlocks!=NULL);
    if(enable_ordinary==1){
        assert(curFunction->live_interval!=NULL);
        build_live_interval(curFunction->ToPoBlocks,curFunction->live_interval);
    }

//    printf("pqueuesize =%d\n", PriorityQueueSize(queue));
    if(enable_vfp==1){
        assert(curFunction->vfp_live_interval!=NULL);
        build_vfp_live_interval(curFunction->ToPoBlocks,curFunction->vfp_live_interval);
    }
    if(enable_ordinary==1){
        print_live_interval(curFunction->live_interval);
    }
    if(enable_vfp==1){
        printf("Var_Float:\n");
        print_live_interval(curFunction->vfp_live_interval);
    }
}


void line_scan(InstNode*ins,Function* start){
    Function *curFunction=start;
    for(;curFunction!=NULL;curFunction=curFunction->Next){
        if(enable_ordinary==1){
//        初始化函数的活跃变量表
            curFunction->live_interval=PriorityQueueInit();
            curFunction->live_interval->set_compare(curFunction->live_interval,CompareNumerics);
//        初始化函数的寄存器分配结果表
            curFunction->lineScanReg=HashMapInit();
        }

        if(enable_vfp==1){
            curFunction->vfp_live_interval=PriorityQueueInit();
            curFunction->vfp_live_interval->set_compare(curFunction->vfp_live_interval,CompareNumerics);
            curFunction->lineScanVFPReg=HashMapInit();
        }
//        获取函数的活跃变量表
        get_function_live_interval(curFunction);
//        active <-- {}
        if(enable_ordinary==1){
            active=PriorityQueueInit();
            active->set_compare(active,CompareNumerics2);
            location=HashSetInit();
        }
        if(enable_vfp==1){
            VFPactive=PriorityQueueInit();
            VFPactive->set_compare(VFPactive,CompareNumerics2);
            VFPlocation=HashSetInit();
        }

        memset(myreg,0,sizeof(myreg));
        free_reg_num=11;
        memset(VFPreg,0, sizeof(VFPreg));
        free_VFPreg_num=26;
//      到这里为止

        if(enable_ordinary==1){
            line_scan_alloca(curFunction,curFunction->live_interval);
        }
        if(enable_vfp==1){
            VFP_line_scan_alloca(curFunction,curFunction->vfp_live_interval);
        }

        label_the_result_of_linescan_register(curFunction,curFunction->entry->head_node);




        Pair *ptr_pair;
        void *elem;
        value_live_range *tmp;
//      打印寄存器分配的结果,打印通用寄存器
        if(enable_ordinary==1){
            printf("in register:\n");
            HashMapFirst(curFunction->lineScanReg);
            while ((ptr_pair= HashMapNext(curFunction->lineScanReg))!=NULL){
                Value *value=(Value*)ptr_pair->key;
                value_register *node=(value_register*)ptr_pair->value;
                printf("%s --> r%d\n",value->name,node->reg);
            }
            printf("in memory:\n");
            HashSetFirst(location);
            while ((elem= HashSetNext(location))!=NULL){
                tmp=(value_live_range*)elem;
                printf("%s statr %d,end %d\n",tmp->value->name,tmp->start,tmp->end);
            }
        }
        if(enable_vfp==1){
//            打印浮点寄存器的分配结果
            printf("Var_Float in register:\n");
            HashMapFirst(curFunction->lineScanVFPReg);
            while ((ptr_pair= HashMapNext(curFunction->lineScanVFPReg))!=NULL){
                Value *value=(Value*)ptr_pair->key;
                value_register *node=(value_register*)ptr_pair->value;
                printf("%s --> s%d\n",value->name,node->sreg);
            }
            printf("Var_Float in memory:\n");
            HashSetFirst(VFPlocation);
            while ((elem= HashSetNext(VFPlocation))!=NULL){
                tmp=(value_live_range*)elem;
                printf("%s statr %d,end %d\n",tmp->value->name,tmp->start,tmp->end);
            }
        }



//        在这里可以把active,location,VFPactive和VFPlocation释放掉
        if(enable_ordinary==1){
            PriorityQueueDeinit(active);
            HashSetDeinit(location);
        }
        if(enable_vfp==1){
            PriorityQueueDeinit(VFPactive);
            HashSetDeinit(VFPlocation);
        }
    }
}

void line_scan_alloca(Function *curFunction,PriorityQueue*queue){
    void *elem;
    value_live_range *i;
    printf("PriorityQueueSize(queue) =%d\n",PriorityQueueSize(queue));
    while (PriorityQueueSize(queue)!=0){ //foreach live interval i

        PriorityQueueTop(queue,&elem);
        PriorityQueuePop(queue);

        i=(value_live_range*)elem;
//        printf("analyze %s\n",i->value->name);
        expire_old_intervals(curFunction,i);
        if(PriorityQueueSize(active)==R){
            spill_at_interval(curFunction,i);
        }else{
            value_register *r=(value_register*) malloc(sizeof(value_register));
            memset(r,0, sizeof(value_register));
            r->reg=get_an_availabel_register();
            assert(r->reg!=-1);
            HashMapPut(curFunction->lineScanReg,i->value,r);
//            printf("put curFunction->lineScanReg %s\n",i->value->name);
//            printf("%s\n",i->value->name);
            PriorityQueuePush(active,i);
        }
    }

}
void expire_old_intervals(Function *curFunction,value_live_range *i){
//    foreach interval j in active, in order of increasing end point
    while (PriorityQueueSize(active)!=0){
        void *elem;
        value_live_range *j;
        PriorityQueueTop(active,&elem);
        j=(value_live_range*)elem;
//        if endpoint[j] ≥ startpoint[i]
#ifdef YAOWEI_TEST
        if((j->end>i->start)||(!(j->end==i->start)&&(j->lastisuse&&i->firstisdef))){
            return;
#else
        if(j->end>=i->start){ //等号删除是有点问题的
            return;
#endif
        }else{
//            remove j from active
            PriorityQueuePop(active);
//            add register[j] to pool of free registers
//             j已经被进行寄存器分配
            value_register *r= HashMapGet(curFunction->lineScanReg,j->value);
            assert(r!=NULL);
            free_register(r->reg);
        }
    }
}
void spill_at_interval(Function *curFunction,value_live_range *i){
    value_live_range *spill=get_last_interval_in_active(active);
    if(spill->end>i->end){
        printf("spill %s\n",spill->value->name);
        value_register *r = HashMapGet(curFunction->lineScanReg,spill->value);
        assert(r!=NULL);
        HashMapRemove(curFunction->lineScanReg,spill->value);

        value_register *node=(value_register*) malloc(sizeof(value_register));
        memset(node,0, sizeof(value_register));
        node->reg=r->reg;
        assert(node->reg!=-1);
        HashMapPut(curFunction->lineScanReg,i->value,node);

        HashSetAdd(location,spill);
        pop_last_interval_in_active(active,spill);

        PriorityQueuePush(active,i);
    } else{
        HashSetAdd(location,i);
    }
}

//处理浮点寄存器分配
void VFP_line_scan_alloca(Function *curFunction,PriorityQueue*queue){
    void *elem;
    value_live_range *i;
    printf("Var_Float PriorityQueueSize(queue) =%d\n",PriorityQueueSize(queue));
    while (PriorityQueueSize(queue)!=0){ //foreach live interval i

        PriorityQueueTop(queue,&elem);
        PriorityQueuePop(queue);

        i=(value_live_range*)elem;
//        printf("analyze %s\n",i->value->name);
        VFP_expire_old_intervals(curFunction,i);
        if(PriorityQueueSize(VFPactive)==S){
            VFP_spill_at_interval(curFunction,i);
        }else{
            value_register *r=(value_register*) malloc(sizeof(value_register));
            memset(r,0, sizeof(value_register));
            r->sreg=get_an_availabel_VFPregister();
            assert(r->sreg!=-1);
            HashMapPut(curFunction->lineScanVFPReg,i->value,r);
//            printf("put curFunction->lineScanReg %s\n",i->value->name);
//            printf("%s\n",i->value->name);
            PriorityQueuePush(VFPactive,i);
        }
    }

}
void VFP_expire_old_intervals(Function *curFunction,value_live_range *i){
//    foreach interval j in active, in order of increasing end point
    while (PriorityQueueSize(VFPactive)!=0){
        void *elem;
        value_live_range *j;
        PriorityQueueTop(VFPactive,&elem);
        j=(value_live_range*)elem;
//        if endpoint[j] ≥ startpoint[i]
#ifdef YAOWEI_TEST
        if((j->end>i->start)||(!(j->end==i->start)&&(j->lastisuse&&i->firstisdef))){
            return;
#else
        if(j->end>=i->start){
            return;
#endif
        }else{
//            remove j from active
            PriorityQueuePop(VFPactive);
//            add register[j] to pool of free registers
//             j已经被进行寄存器分配
            value_register *r= HashMapGet(curFunction->lineScanVFPReg,j->value);
            assert(r!=NULL);
//            printf("VFP_expire_old_intervals free s%d\n",r->sreg);
            free_VFPregister(r->sreg);
        }
    }
}
void VFP_spill_at_interval(Function *curFunction,value_live_range *i){
    value_live_range *spill=get_last_interval_in_active(VFPactive);
    if(spill->end>i->end){
        printf("spill %s\n",spill->value->name);
        value_register *r = HashMapGet(curFunction->lineScanVFPReg,spill->value);
        assert(r!=NULL);
        HashMapRemove(curFunction->lineScanVFPReg,spill->value);

        value_register *node=(value_register*) malloc(sizeof(value_register));
        memset(node,0, sizeof(value_register));
        node->sreg=r->sreg;
        assert(node->sreg!=-1);
        HashMapPut(curFunction->lineScanVFPReg,i->value,node);

        HashSetAdd(VFPlocation,spill);
        pop_last_interval_in_active(VFPactive,spill);

        PriorityQueuePush(VFPactive,i);
    } else{
        HashSetAdd(VFPlocation,i);
    }
}

void label_the_result_of_linescan_register(Function *curFunction,InstNode * ins){
    Value *value0,*value1,*value2;
    int opNum;
    while (ins->inst->Opcode!=FunEnd){
        switch (ins->inst->Opcode) {
            case Add:
            case Sub:
            case Mul:
            case Div:
            case Mod:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                label_register(curFunction,ins,value2,2);
                break;
            case Call:
                if(!returnValueNotUsed(ins)){
                    value0=&ins->inst->user.value;
                    label_register(curFunction,ins,value0,0);
                }
                break;
//        case FunBegin:
//            break;
            case Return:
                opNum=ins->inst->user.value.NumUserOperands;
                if(opNum!=0){
                    value1= user_get_operand_use(&ins->inst->user,0)->Val;
                    label_register(curFunction,ins,value1,1);
                }
                break;
            case Store:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                label_register(curFunction,ins,value1,1);
                label_register(curFunction,ins,value2,2);
                break;
            case Load:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                break;
//        case Alloca:
//            value0 = &ins->inst->user.value;
//            break;
            case GIVE_PARAM:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(curFunction,ins,value1,1);
                break;
//        case ALLBEGIN:
//            break;
            case LESS:
            case GREAT:
            case LESSEQ:
            case GREATEQ:
            case EQ:
            case NOTEQ:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                label_register(curFunction,ins,value2,2);
                break;
            case br_i1:
//            value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val; //真值
                label_register(curFunction,ins,value1,1);
                break;
//        case br:
//            break;
//        case Label:
//            break;
            case XOR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                break;
//        case zext:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            break;
//        case bitcast:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            break;
            case GEP:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                label_register(curFunction,ins,value2,2);
                break;
//        case MEMCPY:
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case MEMSET:
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case zeroinitializer:
//            break;
//        case GLOBAL_VAR:
//            value0=&ins->inst->user.value;
//            value1=user_get_operand_use(&ins->inst->user,0)->Val;
//            value2= user_get_operand_use(&ins->inst->user,1)->Val;
//            break;
//        case FunEnd:
//            break;
            case CopyOperation:
                value0=ins->inst->user.value.alias;//这里是需要进到alias里面的
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                break;
            case fptosi:
            case sitofp:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(curFunction,ins,value0,0);
                label_register(curFunction,ins,value1,1);
                break;
//        case SysYMemset:
//            break;
            default:
                break;
        }
        ins= get_next_inst(ins);
    }
}
void label_register(Function *curFunction,InstNode *ins,Value *value,int i){
    if(enable_ordinary==1){
        assert(curFunction->lineScanReg!=NULL);
    }
    if(enable_vfp==1){
        assert(curFunction->lineScanVFPReg!=NULL);
    }
//    标浮点数
    if(enable_vfp==1 && isLocalVarFloatType(value->VTy)){
        value_register *node= HashMapGet(curFunction->lineScanVFPReg,value);
        if(node==NULL){
            if(i==0){
                ins->inst->_vfpReg_[i]=-4;
            }else if(i==1){
                ins->inst->_vfpReg_[i]=104;
            } else if(i==2){
                ins->inst->_vfpReg_[i]=105;
            }
            return;
        }
        ins->inst->_vfpReg_[i]=node->sreg;
        return;
    }
//    标通用
    if(enable_ordinary==1){
        value_register *node= HashMapGet(curFunction->lineScanReg,value);
        if(node==NULL){
            if(isImmIntType(value->VTy) || isImmFloatType(value->VTy)){
                return;
            }
            if(isGlobalArrayFloatType(value->VTy)|| isGlobalArrayIntType(value->VTy) || isGlobalVarFloatType(value->VTy) ||
               isGlobalVarIntType(value->VTy)){
                if(enable_globalVar==0){ //不为全局变量地址标寄存器
                    return;
                }
            }
            if(isLocalArrayFloatType(value->VTy) || isLocalArrayIntType(value->VTy)){
                return;
            }
            if(flag_lr==1){
                if(i==0){
                    ins->inst->_reg_[i]=-1;
                }else if(i==1){
                    ins->inst->_reg_[i]=101;
                } else if(i==2){
                    ins->inst->_reg_[i]=100;
                }
            }else{
                if(i==0){
                    ins->inst->_reg_[i]=-10;
                }else if(i==1){
                    ins->inst->_reg_[i]=110;
                } else if(i==2){
                    ins->inst->_reg_[i]=112;
                }
            }
            return;
        }
        ins->inst->_reg_[i]=node->reg;
        return;
    }

}


int get_an_availabel_register(){
    if(flag_lr==1 && flag_r11==1){
        for(int i=3;i<=12;i++){
            if(myreg[i]==0){
                myreg[i]=1;
                free_reg_num--;
                return i;
            }
        }
        if(myreg[14]==0){
            return 14;
        }
        return -1;
    }else if(flag_lr==1 && flag_r11==0){
        for(int i=3;i<=10;i++){
            if(myreg[i]==0){
                myreg[i]=1;
                free_reg_num--;
                return i;
            }
        }
        return -1;
    }
    else{
        for(int i=3;i<10;i++){
            if(myreg[i]==0){
                myreg[i]=1;
                free_reg_num--;
                return i;
            }
        }
        return -1;
    }

}
void free_register(int i){
    free_reg_num++;
    myreg[i]=0;
}
int get_an_availabel_VFPregister(){
//    for(int i=6;i<=31;i++){
//        if(VFPreg[i]==0){
//            VFPreg[i]=1;
//            free_VFPreg_num--;
//            return i;
//        }
//    }
    for(int i=31;i>=6;i--){
        if(VFPreg[i]==0){
            VFPreg[i]=1;
            free_VFPreg_num--;
            return i;
        }
    }
    return -1;
}
void free_VFPregister(int i){
    free_VFPreg_num++;
    VFPreg[i]=0;
}
int CompareNumerics(const void* lhs, const void* rhs){
    value_live_range * x1=(value_live_range *)lhs;
    value_live_range * x2=(value_live_range *)rhs;
    if(x1->start==x2->start){
        return 0;
    }
    return x1->start>=x2->start ? 1 : (-1);
}
int CompareNumerics2(const void* lhs, const void* rhs){
    value_live_range * x1=(value_live_range *)lhs;
    value_live_range * x2=(value_live_range *)rhs;
    if(x1->end==x2->end){
        return 0;
    }
    return x1->end>=x2->end ? 1 : (-1);
}
value_live_range* get_last_interval_in_active(PriorityQueue * curActive){
    HashSet *tmp=HashSetInit();
    void *elem;
    while (PriorityQueueSize(curActive)!=0){
        PriorityQueueTop(curActive,&elem);
        PriorityQueuePop(curActive);
        HashSetAdd(tmp,elem);
    }
    value_live_range *ans=(value_live_range*)elem;
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(curActive,elem);
    }
    return ans;
}
void pop_last_interval_in_active(PriorityQueue * curActive,value_live_range*spill){
    HashSet *tmp=HashSetInit();
    void *elem;
    value_live_range *x;
    while (PriorityQueueSize(curActive)!=0){
        PriorityQueueTop(curActive,&elem);
        PriorityQueuePop(curActive);
        x=(value_live_range*)elem;
        if(x!=spill){
            HashSetAdd(tmp,elem);
        }
    }
    x=(value_live_range*)elem;
    printf("true spill %s\n",x->value->name);
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(curActive,elem);
    }
    HashSetDeinit(tmp);
}

