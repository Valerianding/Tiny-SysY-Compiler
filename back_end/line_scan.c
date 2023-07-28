//
// Created by Administrator on 2023/7/27.
//
#include "line_scan.h"

#define R 6

//还需要一个active(这个可以是PriorityQueue)，和location(这个可以是HashSet)。
PriorityQueue *active;
//最终的寄存器分配方案，location中的成员代表在内存，result中成员代表成功分配寄存器func->lineScanReg
HashSet *location;
//HashMap *result; //key =Value* value=value_register* (int)
Function *func;
int myreg[16];
int free_reg_num;
int get_an_availabel_register(){
    for(int i=4;i<10;i++){
        if(myreg[i]==0){
            myreg[i]=1;
            free_reg_num--;
            return i;
        }
    }
    return -1;
}
void free_register(int i){
    free_reg_num++;
    myreg[i]=0;
}
int CompareNumerics2(const void* lhs, const void* rhs){
    value_live_range * x1=(value_live_range *)lhs;
    value_live_range * x2=(value_live_range *)rhs;
    if(x1->end==x2->end){
        return 0;
    }
    return x1->end>=x2->end ? 1 : (-1);
}

//传入的是FuncBegin的ins
PriorityQueue *get_function_live_interval(InstNode*ins){
    BasicBlock *block;
    Function *function;
    block=ins->inst->Parent;
    function=block->Parent;
    assert(function->ToPoBlocks!=NULL);

    PriorityQueue *queue=PriorityQueueInit();
    queue->set_compare(queue,CompareNumerics);

    assert(queue!=NULL);
    build_live_interval(function->ToPoBlocks,queue);
//    printf("pqueuesize =%d\n", PriorityQueueSize(queue));
//    print_live_interval(queue);
    return queue;
}
void expire_old_intervals(value_live_range *i){
//    foreach interval j in active, in order of increasing end point
    while (PriorityQueueSize(active)!=0){
        void *elem;
        value_live_range *j;
        PriorityQueueTop(active,&elem);
        j=(value_live_range*)elem;
//        if endpoint[j] ≥ startpoint[i]
        if(j->end>=i->start){
            return;
        }else{
//            remove j from active
            PriorityQueuePop(active);
//            add register[j] to pool of free registers
            value_register *r= HashMapGet(func->lineScanReg,j->value);
            free_register(r->reg);
        }
    }
}
void spill_at_interval(value_live_range *i){
    value_live_range *spill=get_last_interval_in_active();
    if(spill->end>i->end){
        value_register *r= HashMapGet(func->lineScanReg,spill->value);
        HashMapRemove(func->lineScanReg,spill->value);

        value_register *node=(value_register*) malloc(sizeof(value_register));
        node->reg=r->reg;
        assert(node->reg!=-1);
        HashMapPut(func->lineScanReg,i->value,node);

        HashSetAdd(location,spill);
        pop_last_interval_in_active();
        PriorityQueuePush(active,i);
    } else{
        HashSetAdd(location,i);
    }
}
value_live_range* get_last_interval_in_active(){
    HashSet *tmp=HashSetInit();
    void *elem;
    while (PriorityQueueSize(active)!=0){
        PriorityQueueTop(active,&elem);
        PriorityQueuePop(active);
        HashSetAdd(tmp,elem);
    }
    value_live_range *ans=(value_live_range*)elem;
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(active,elem);
    }
    return ans;
}
void pop_last_interval_in_active(){
    HashSet *tmp=HashSetInit();
    void *elem;
    while (PriorityQueueSize(active)!=0){
        PriorityQueueTop(active,&elem);
        PriorityQueuePop(active);
        if(PriorityQueueSize(active)!=0){
            HashSetAdd(tmp,elem);
        }
    }
    HashSetFirst(tmp);
    while ((elem= HashSetNext(tmp))!=NULL){
        PriorityQueuePush(active,elem);
    }
}
void line_scan_alloca(PriorityQueue*queue){
    void *elem;
    value_live_range *i;
    printf("PriorityQueueSize(queue) =%d\n",PriorityQueueSize(queue));
    while (PriorityQueueSize(queue)!=0){ //foreach live interval i

        PriorityQueueTop(queue,&elem);

        PriorityQueuePop(queue);
        i=(value_live_range*)elem;
        expire_old_intervals(i); 
        if(PriorityQueueSize(active)==R){
            spill_at_interval(i);
        }else{
            value_register *r=(value_register*) malloc(sizeof(value_register));
            r->reg=get_an_availabel_register();
            assert(r->reg!=-1);
            HashMapPut(func->lineScanReg,i->value,r);
//            printf("%s\n",i->value->name);
            PriorityQueuePush(active,i);
        }
    }
//    PriorityQueueDeinit(active);
//    HashSetDeinit(location);
}

void line_scan(InstNode*ins){
    PriorityQueue *queue;
    while (ins!=NULL){
        if(ins->inst->Opcode==FunBegin){
            printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
            queue= get_function_live_interval(ins); //获取该函数的live_interval
//            active <-{}
            active=PriorityQueueInit();
            active->set_compare(active,CompareNumerics2);
            location=HashSetInit();
            func=ins->inst->Parent->Parent;
            assert(func!=NULL);
            func->lineScanReg=HashMapInit();

            memset(myreg,0,sizeof(myreg));
            free_reg_num=7;

            line_scan_alloca(queue);
//            printf("now !\n");
//            label_the_result_of_linescan_register(ins);

        }
        if(ins->inst->Opcode==FunEnd){
            //            把线性扫描的寄存器分配结果标到每条ir中，寄存器分配的结果在result（分配寄存器）和location（在内存中）
//            打印该信息
            printf("in register:\n");
            Pair *ptr_pair;
            HashMapFirst(func->lineScanReg);
            while ((ptr_pair= HashMapNext(func->lineScanReg))!=NULL){
                Value *value=(Value*)ptr_pair->key;
                value_register *node=(value_register*)ptr_pair->value;
                printf("%s --> r%d\n",value->name,node->reg);
            }
            printf("in memory:\n");
            void *elem;
            value_live_range *tmp;
            HashSetFirst(location);
            while ((elem= HashSetNext(location))!=NULL){
                tmp=(value_live_range*)elem;
                printf("%s statr %d,end %d\n",tmp->value->name,tmp->start,tmp->end);
            }

//          释放，如果说需要mov r6,r0这样子标的话，感觉这个result的信息得留到arm中翻译的的时候去使用
            PriorityQueueDeinit(active);
//            HashMapDeinit(result);
            HashSetDeinit(location);
        }

        ins= get_next_inst(ins);

    }
}
void label_the_result_of_linescan_register(InstNode * ins){
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
                label_register(ins,value0,0);
                label_register(ins,value1,1);
                label_register(ins,value2,2);
                break;
            case Call:
//            printf("call %d\n",ins->inst->i);
                if(!returnValueNotUsed(ins)){
                    value0=&ins->inst->user.value;
//                printf("%s call def\n",value0->name);
                    label_register(ins,value0,0);
                }
//            printf("call\n");
                break;
//        case FunBegin:
//            break;
            case Return:
                opNum=ins->inst->user.value.NumUserOperands;
                if(opNum!=0){
                    value1= user_get_operand_use(&ins->inst->user,0)->Val;
                    label_register(ins,value1,1);
                }
//            printf("return\n");
                break;
            case Store:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                value2= user_get_operand_use(&ins->inst->user,1)->Val;
                label_register(ins,value1,1);
                label_register(ins,value2,2);
//            printf("store\n");
                break;
            case Load:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(ins,value0,0);
                label_register(ins,value1,1);
//            printf("load\n");
                break;
//        case Alloca:
//            value0 = &ins->inst->user.value;
//            break;
            case GIVE_PARAM:
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(ins,value1,1);
//            printf("give_param\n");
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
                label_register(ins,value0,0);
                label_register(ins,value1,1);
                label_register(ins,value2,2);
//            printf("noteq\n");
                break;
            case br_i1:
//            value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val; //真值
                label_register(ins,value1,1);
                break;
//        case br:
//            break;
//        case Label:
//            break;
            case XOR:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(ins,value0,0);
                label_register(ins,value1,1);
//            printf("xor\n");
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
                label_register(ins,value0,0);
                label_register(ins,value1,1);
                label_register(ins,value2,2);
//            printf("gep\n");
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
                label_register(ins,value0,0);
                label_register(ins,value1,1);
//            printf("copy\n");
                break;
            case fptosi:
            case sitofp:
                value0=&ins->inst->user.value;
                value1=user_get_operand_use(&ins->inst->user,0)->Val;
                label_register(ins,value0,0);
                label_register(ins,value1,1);
//            printf("sitofp\n");
                break;
//        case SysYMemset:
//            break;
            default:
                break;
        }
        ins= get_next_inst(ins);
    }
}
void label_register(InstNode *ins,Value *value,int i){
    assert(func->lineScanReg!=NULL);
    value_register *node= HashMapGet(func->lineScanReg,value);
    if(node==NULL){
        if(i==0){
            ins->inst->_reg_[i]=-10;
        }else if(i==1){
            ins->inst->_reg_[i]=110;
        } else if(i==2){
            ins->inst->_reg_[i]=112;
        }
        return;
    }
    ins->inst->_reg_[i]=node->reg;
}