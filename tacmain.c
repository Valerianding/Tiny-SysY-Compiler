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
    Instruction *ins1 = ins_new_unary_operator(ADD,&a);
    Instruction *ins2 = ins_new_binary_operator(ADD,(Value*)ins1,&b);

    //
    printf("pass phrase 1\n");


    BasicBlock block;
    bblock_init(&block, NULL);
    printf("block init\n");
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