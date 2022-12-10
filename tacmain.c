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
    Instruction *ins2 = ins_new_binary_operator(Add,(Value*)ins1,&b);


    printf("in/out: %d\n",ins1->user.value.is_in);
    printf("in/out: %d\n",ins1->user.value.is_out);

    printf("pass phrase 1\n");


    BasicBlock block;
    bblock_init(&block, NULL);
    printf("block init\n");
    printf("%d\n",ins1->Parent);


    InstNode *temp = new_inst_node(ins1);
    InstNode *temp1 = new_inst_node(ins2);

    ins_node_add(temp,temp1);
    printf("%p\n",temp->list.next);
    printf("%p\n",&temp1->list);

    bb_set_block(&block,temp,temp1);
    assert(&block == temp->inst->Parent);
    assert(&block == temp1->inst->Parent);

    ins1->i = 1;
    InstNode *ret = search_inst_node(temp,1);
    printf("%p\n",ret);
    printf("%p\n",temp);

//    assert(sc_list_count(&(block.inst_list)->list) == 1);
//    printf("%p\n",block.inst_list->inst);
//    printf("%p\n",ins2);
//
//    Instruction *op_ins_copy = bblock_pop_inst_back(&block);
//    assert(sc_list_count(&(block.inst_list)->list) == 0);
//    assert(op_ins_copy == ins2);
//    assert(op_ins_copy->user.value.NumUserOperands == 2);


//    sc_list 的有问题
//    struct sc_list temp;
//    sc_list_init(&temp);
//    size_t count = sc_list_count(&temp);
//    printf("%zu\n",count);
//    printf("Unit Test Passed!\n");
}