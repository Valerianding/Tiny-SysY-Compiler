//
// Created by Administrator on 2023/7/27.
//
#include "line_scan.h"

#define R 6

//还需要一个active(这个可以是PriorityQueue)，和location(这个可以是HashSet)。
PriorityQueue *active;
//最终的寄存器分配方案，location中的成员代表在内存，result中成员代表成功分配寄存器
HashSet *location;
HashMap *result; //key =Value* value=value_register* (int)
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
    PriorityQueue *queue;
    queue= build_live_interval(function->ToPoBlocks);
//    printf("pqueuesize =%d\n", PriorityQueueSize(queue));
    print_live_interval();
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
            value_register *r= HashMapGet(result,j->value);
            free_register(r->reg);
        }
    }
}
void spill_at_interval(value_live_range *i){
    value_live_range *spill=get_last_interval_in_active();
    if(spill->end>i->end){
        value_register *r= HashMapGet(result,spill->value);
        HashMapRemove(result,spill->value);

        value_register *node=(value_register*) malloc(sizeof(value_register));
        node->reg=r->reg;
        assert(node->reg!=-1);
        HashMapPut(result,i->value,node);

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
            HashMapPut(result,i->value,r);
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
            result=HashMapInit();

            memset(myreg,0,sizeof(myreg));
            free_reg_num=7;

            line_scan_alloca(queue);
//            把线性扫描的寄存器分配结果标到每条ir中，寄存器分配的结果在result（分配寄存器）和location（在内存中）
//            打印该信息
            printf("in register:\n");
            Pair *ptr_pair;
            HashMapFirst(result);
            while ((ptr_pair= HashMapNext(result))!=NULL){
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

//          释放
//            PriorityQueueDeinit(active);
//            HashMapDeinit(result);
//            HashSetDeinit(location);
        }
        ins= get_next_inst(ins);
    }
}