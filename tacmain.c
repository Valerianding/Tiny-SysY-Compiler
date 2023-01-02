#include <assert.h>
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "function.h"
#include "stdio.h"
#include "liveness.h"

//FIXME: test purpose only!
Symtab* test_symtab;


int main(){
    
    // 初始化工作
    //test_symtab = (Symtab*)malloc(sizeof(Symtab));
    //symtab_init((&test_symtab));
    Value a,b;
    /* 注意所有的value都需要初始化 */
    value_init_int(&a,10);
    value_init_int(&b,20);
    Instruction *ins1 = ins_new_unary_operator(Add,&a);
    Instruction *ins2 = ins_new_unary_operator(Assign,&a);
    Instruction *ins3 = ins_new_unary_operator(Assign,&a);
    Instruction *ins4 = ins_new_goto_operator(1);
    Instruction *ins5 = ins_new_binary_operator(Add,&a,&b);
    Instruction *ins6 = ins_new_binary_operator(Return,&a,&b);
    printf("block init\n");


    InstNode *temp  = new_inst_node(ins1);
    InstNode *temp1 = new_inst_node(ins2);
    InstNode *temp2 = new_inst_node(ins3);
    InstNode *temp3 = new_inst_node(ins4);
    InstNode *temp4 = new_inst_node(ins5);
    InstNode *temp5 = new_inst_node(ins6);


    struct sc_list *test;
    test = sc_list_tail(&temp->list);
    printf(" tail : %p\n",test);
    ins_node_add(temp,temp1);
    test = sc_list_tail(&temp->list);
    printf(" tail : %p\n",test);
    ins_node_add(temp,temp2);
    test = sc_list_tail(&temp->list);
    printf(" tail : %p\n",test);
    ins_node_add(temp,temp3);
    ins_node_add(temp,temp4);
    ins_node_add(temp,temp5);

    printf("add OK\n");
    temp->inst->i = 1;
    temp1->inst->i = 2;
    temp2->inst->i = 3;
    temp3->inst->i = 4;
    temp4->inst->i = 5;
    temp5->inst->i = 6;
    //
    temp5->inst->user.value.is_last = 1;
    printf("%d\n",ins1->i);
    printf("%d\n",ins2->i);
    printf("%d\n",ins3->i);
    printf("%d\n",ins4->i);
    printf("%d\n",ins5->i);

    printf("%p\n",temp->list.next);
    printf("%p\n",&temp1->list);

    ins1->i = 1;
    InstNode *ret = search_inst_node(temp,1);
    printf("%p\n",ret);
    printf("%p\n",temp);


    InstNode *now = get_next_inst(temp4);
    if(now == NULL){
        printf("correct!\n");
    }else{
        printf("%p",now);
    }

    bblock_divide(temp);
    printf("------------\n");
    printf("%p\n",ins1->Parent);
    printf("%p\n",ins2->Parent);
    printf("%p\n",ins3->Parent);
    printf("%p\n",ins4->Parent);
    printf("%p\n",ins5->Parent);
    printf("%p\n",ins6->Parent);

    printf("-----pharse1 done-----\n");
//    ins2->value_VarSpace = HashMapInit();
//    ins1->value_VarSpace = HashMapInit();
//    VarSpace *test_space = (VarSpace*)malloc(sizeof(VarSpace));
//    test_space->isLive = true;
//    HashMapPut(ins2->value_VarSpace,&a,test_space);

    /* temp -> temp1 -> temp2 */
    Value c;
    BasicBlock test_block;
    test_block.tail_node = temp1;
    test_block.head_node = temp;
    temp->inst->dest = &c;
    temp1->inst->dest = &c;
    a.name = (char*)malloc(sizeof(char) * 3);
    b.name = (char*)malloc(sizeof(char) * 3);
    c.name = (char*)malloc(sizeof(char) * 3);
    a.name = "a";
    b.name = "b";
    c.name = "c";
    /*          in
     * c = a + b
     *          in
     * a = b + c
     *          in
     * d =
     */
    ll_analysis(&test_block);
    printliveness(&test_block);
    printf("OK\n");
//    if(HashMapContain(ins2->value_VarSpace,&a)){
//        printf("exsit!\n");
//    }
//    //测试
//    HashMapFirst(temp1->inst->value_VarSpace);
//    for(Pair *pair = HashMapNext(temp1->inst->value_VarSpace); pair != NULL; pair = HashMapNext(temp1->inst->value_VarSpace)){
//        VarSpace *temp = (VarSpace*)malloc(sizeof(VarSpace));
//        VarSpace *pair_value = (VarSpace*)pair->value;
//        temp->isLive = pair_value->isLive;
//        temp->place = pair_value->place;
//        temp->order = pair_value->place;
//        printf("pair_value is live: %d",pair_value->isLive);
//    }
//    HashMapPut(ins1->value_VarSpace,)

//    sc_list 的有问题 已经修改了
//    struct sc_list temp;
//    sc_list_init(&temp);
//    size_t count = sc_list_count(&temp);
//    printf("%zu\n",count);
//    printf("Unit Test Passed!\n");
}