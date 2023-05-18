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
#include "func_inline.h"
//FIXME: test purpose only!
Symtab* test_symtab;

extern int yyparse();
//extern past TRoot;
Symtab *this;
extern FILE *yyin;
int return_index=0;
int return_stmt_num[20]={0};
char t[8];
int t_index = 0;
insnode_stack S_continue;
insnode_stack S_break;
insnode_stack S_return;
insnode_stack S_and;
insnode_stack S_or;

char t_num[7] = {0};
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
    flag_blocklist=1;
    create_instruction_list(TRoot,NULL);
    travel_finish_type(instruction_list);
 //   printf_llvm_ir(instruction_list,argv[1]);
//  print_array(instruction_list);
//  showAst(TRoot,0);
//

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
        removeUnreachable(currentFunction);
        calculate_dominance(currentFunction);
        calculatePostDominance(currentFunction);
        clear_visited_flag(block);
        calculate_dominance_frontier(currentFunction);
        calculate_iDominator(currentFunction);
        calculate_DomTree(currentFunction);
        calculateNonLocals(currentFunction);
        printf("after non locals\n");
    }

    // 建立phi之前
    printf_llvm_ir(instruction_list,argv[1]);

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        mem2reg(currentFunction);
        printf("after one mem2reg Function!\n");
    }

    // 优化之前
    printf_llvm_ir(instruction_list,argv[1]);

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next) {
        Mark(currentFunction);
        Sweep(currentFunction);
        commonSubexpressionElimination(currentFunction);
        //为了loop 作准备
        calculateLiveness(currentFunction);
        printLiveness(currentFunction);
        loop(currentFunction);
        Clean(currentFunction);
        renameVariabels(currentFunction);
    }

     //phi上的优化
    printf_llvm_ir(instruction_list,argv[1]);

    block = temp->inst->Parent;
    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        SSADeconstruction(currentFunction);
        cleanLiveSet(currentFunction);
    }

    //请注释掉我跑llvm脚本
    //printf_llvm_ir(instruction_list,argv[1]);


    //phi后优化


    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        clear_visited_flag(currentFunction->entry);
        printf("after out of SSA!\n");
        calculateLiveness1(currentFunction);
        printLiveness(currentFunction);
    }

    // Liveness 计算之后请注释掉我跑llvm
    //printf_llvm_ir(instruction_list,argv[1]);


    //TODO 目前函数内联放在这里了，暂时的
//    printf("=======func inline=========\n");
//    func_inline(instruction_list);
//    printf_llvm_ir(instruction_list,argv[1]);
//    printf("=======func inline end=======\n");

//
//    //ljw_begin
//   reg_control(instruction_list,temp);
    //修改all_in_memory开启/关闭寄存器分配
    //ljw_end

    //lsy_begin TODO core dump
//    fix_array(instruction_list);
//    printf_llvm_ir(instruction_list,argv[1]);
    //lsy_end

    //    ljf
    // arm_translate_ins(instruction_list);
    return 0;
}
