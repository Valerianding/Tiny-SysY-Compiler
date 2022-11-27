#include <assert.h>
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"

#include "stdio.h"

#include "optimize.h"

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
    Instruction *ins1 = ins_new_unary_operator(ADD,&a);
    Instruction *ins2 = ins_new_binary_operator(ADD,(Value*)ins1,&b);

    //
    printf("pass phrase 1\n");


    BasicBlock block;
    bblock_init(&block, NULL);
    printf("block init\n");

    //printf("%zu\n",sc_list_count(&(block.inst_list)->list));
    bblock_add_inst_back(&block, ins2);
    bblock_add_inst_back(&block,ins1);
    if((block.inst_list)->inst == NULL) printf("Is NULL\n");
    if(block.inst_list == NULL) printf("IS A NULL\n");
    printf("%p\n",block.inst_list->inst);
    printf("%p\n",ins2);
    printf("%p\n", bblock_get_inst_back(&block));
    printf("%p\n",ins1);
    printf("%zu\n",sc_list_count(&(block.inst_list)->list));
    printf("%zu\n", bb_count_ins(&block));

    bblock_pop_inst_back(&block);

    printf("%p\n", bblock_get_inst_back(&block));
    printf("%zu\n",sc_list_count(&(block.inst_list)->list));
    printf("%zu\n", bb_count_ins(&block));

    printf("%p\n",block.user.use_list->Parent);
    printf("%p\n",&block.user);
    printf("%p\n",(block.user.use_list + 1)->Parent);

    BasicBlock block2;
    bblock_init(&block2,NULL);
    moveBefore(&block2,&block);
    printf("%p\n",block2.user.use_list);
    printf("%p\n",block.user.value.use_list);

    printf("-----------------------\n");
    BasicBlock block3;
    bblock_init(&block3,NULL);
    moveAfter(&block3,&block2);


    printf("%p\n",block2.user.use_list + 1);
    printf("%p\n",block3.user.value.use_list);

//    assert(sc_list_count(&(block.inst_list)->list) == 1);
//    printf("%p\n",block.inst_list->inst);
//    printf("%p\n",ins2);
//
//    Instruction *op_ins_copy = bblock_pop_inst_back(&block);
//    assert(sc_list_count(&(block.inst_list)->list) == 0);
//    assert(op_ins_copy == ins2);
//    assert(op_ins_copy->user.value.NumUserOperands == 2);


//sc_list 的有问题
//    struct sc_list temp;
//    sc_list_init(&temp);
//    size_t count = sc_list_count(&temp);
//    printf("%zu\n",count);
//    printf("Unit Test Passed!\n");
}