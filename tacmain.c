#include <assert.h>
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"


//FIXME: test purpose only!
Symtab* test_symtab;


int main(){
    
    // 初始化工作
    symtab_init((&test_symtab));
    
    ConstantNum a, b;
    
    const_init_int(&a, 10);
    const_init_int(&b, 20);
    
    Value var1;
    value_set_name(&var1, "var_name");
    Instruction *op_ins = ins_new_binary_operator('+', (Value*)&a, (Value*)&b);

    BasicBlock block;
    bblock_init(&block, NULL);
    bblock_add_inst_back(&block, op_ins);
    assert(sc_list_count(&block.inst_list) == 1);

    Instruction *op_ins_copy = bblock_pop_inst_back(&block);
    assert(sc_list_count(&block.inst_list) == 0);
    assert(op_ins_copy == op_ins);
    assert(op_ins_copy->user.value.NumUserOperands == 2);
    
}