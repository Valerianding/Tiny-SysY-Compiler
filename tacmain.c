#include <assert.h>
#include "utility.h"
#include "arm.h"
#include "value.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "stdio.h"
#include "bb_divide.h"
#include "dominance.h"
#include "mem2reg.h"
#include "travel.h"
#include "register_allocation.h"
#include "livenessanalysis.h"
#include "PassManager.h"
//FIXME: test purpose only!
Symtab* test_symtab;

extern int yyparse();
//extern past TRoot;
Symtab *this;
extern FILE *yyin;
int return_index=0;
int return_stmt_num[20]={0};
char t[6];
int t_index = 0;
insnode_stack S_continue;
insnode_stack S_break;
insnode_stack S_return;
insnode_stack S_and;
insnode_stack S_or;
bool c_b_flag[2];

char t_num[5] = {0};
int flag_blocklist;

struct _InstNode *instruction_list;

void yyerror(char *s)
{
    printf("%s\n", s);
}

int main(int argc, char* argv[]){
    //lsy
    if(argc < 2 ){
        printf("ERROR: input file name is needed. \n");
        exit(0);
    }
    yyin=fopen(argv[1], "r");

    Instruction *ins_head= ins_new_unary_operator(ALLBEGIN,NULL);
    instruction_list= new_inst_node(ins_head);
    t[0]='%';

    //TODO 还有没有更好的做法呢
    init_insnode_stack(&S_continue);
    init_insnode_stack(&S_break);
    init_insnode_stack(&S_return);
    init_insnode_stack(&S_and);
    init_insnode_stack(&S_or);

    this=(Symtab*) malloc(sizeof(Symtab));
    symtab_init(this);
    yyparse();
    return_index=0;
    TRoot=TRoot->left;
    stack_new(this);
    declare_global_alloca(this->value_maps->next);
    c_b_flag[0]=false;c_b_flag[1]=false;
    flag_blocklist=1;
    create_instruction_list(TRoot,NULL);
    travel_finish_type(instruction_list);
    printf_llvm_ir(instruction_list,argv[1]);
//  fix_array(instruction_list);
//  print_array(instruction_list);`
//  showAst(TRoot,0);

    bblock_divide(instruction_list);


    showInstructionInfo(instruction_list);


    showBlockInfo(instruction_list);


    InstNode *temp = get_next_inst(instruction_list);
    //找到第一个function的
    while(temp->inst->Parent->Parent == NULL){
        temp = get_next_inst(temp);
    }
    BasicBlock *block = temp->inst->Parent;


    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        printf("-------function  start---------\n");
        correctType(currentFunction);
        print_function_info(currentFunction);
        clear_visited_flag(block);
        calculate_dominance(currentFunction);
        clear_visited_flag(block);
        calculate_dominance_frontier(currentFunction);
        calculate_iDominator(currentFunction);
        calculate_DomTree(currentFunction);
        calculateNonLocals(currentFunction);
        printf("after non locals\n");
        mem2reg(currentFunction);
        printf("------after a function------\n");
        loop(currentFunction);
        //RunPasses(currentFunction);
    }

    // 建立phi 之后的
    printf_llvm_ir(instruction_list,argv[1]);

    InstNode *temp2 = instruction_list;
    /* 测试所有instruction list */
    for(;temp2 != NULL;temp2 = get_next_inst(temp2)){
        print_one_ins_info(temp2);
    }

    block =  get_next_inst(instruction_list)->inst->Parent;
    clear_visited_flag(block);
    print_block_info(block);

    //找到第一个function的
    while(temp->inst->Parent->Parent == NULL){
        temp = get_next_inst(temp);
    }

    block = temp->inst->Parent;
    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        SSADeconstruction(currentFunction);
        clear_visited_flag(currentFunction->entry);
        printf("after out of SSA!\n");
        calculateLiveness(currentFunction);
        printLiveness(currentFunction->entry);
   }

    // 消除phi函数之后
    //printf_llvm_ir(instruction_list,argv[1]);

    //ljw_begin
    // reg_control();
    //修改all_in_memory开启/关闭寄存器分配
    //ljw_end
    //    ljf
    //arm_translate_ins(instruction_list);
    return 0;
}