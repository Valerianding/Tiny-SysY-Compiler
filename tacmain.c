#include <assert.h>
#include "utility.h"
#include "arm.h"
#include "value.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "stdio.h"
#include "bbdivide.h"
#include "dominance.h"
#include "travel.h"
#include "register_allocation.h"
#include "livenessanalysis.h"
#include "PassManager.h"
#include "mem2reg.h"
#include "sideeffect.h"

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
    srand(time(NULL));
    //lsy
    if(argc < 2 ){
        printf("ERROR: input file name is needed. \n");
        exit(0);
    }
    yyin=fopen(argv[4], "r");

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
    create_instruction_list(TRoot,NULL,0);
    travel_finish_type(instruction_list);
    move_give_param(instruction_list);
    printf_llvm_ir(instruction_list,argv[4],1);
//  print_array(instruction_list);
    // showAst(TRoot,0);

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
        dominanceAnalysis(currentFunction);
    }

    //建立phi之前
    printf_llvm_ir(instruction_list,argv[4],1);

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        calculateNonLocals(currentFunction);
        mem2reg(currentFunction);
        calculateLiveness(currentFunction);
        printLiveness(currentFunction);
    }

    //mem2reg之后，优化前
    printf_llvm_ir(instruction_list,argv[4],1);

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next) {
//        sideEffect(currentFunction);
//        ConstFolding(currentFunction);
//        commonSubexpressionElimination(currentFunction);
//        memlvn(currentFunction);

//
        //DVNT(currentFunction);
//
//        //loop
//        loop(currentFunction);
//        LICM(currentFunction);
//
//
//        //dce
//        Mark(currentFunction);
//        Sweep(currentFunction);
//
//        //CfgSimplify
//        Clean(currentFunction);
        renameVariables(currentFunction);
    }

    printf_llvm_ir(instruction_list,argv[4],1);


    //基本块内inscomb ok，基本块间ing
//    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
//        instruction_combination(currentFunction);
//        renameVariables(currentFunction);
//    }

    //phi上的优化
    //printf_llvm_ir(instruction_list,argv[4],1);

    block = temp->inst->Parent;
    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        SSADeconstruction(currentFunction);
        renameVariables(currentFunction);
        cleanLiveSet(currentFunction);
    }

    //请注释掉我跑llvm脚本 phi函数消除
    printf_llvm_ir(instruction_list,argv[4],1);


    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        printf("function: %s\n",currentFunction->entry->head_node->inst->user.use_list[0].Val->name);
        clear_visited_flag(currentFunction->entry);
        printf("after out of SSA!\n");
        calculateLiveness(currentFunction);
        printLiveness(currentFunction);
    }




    // Liveness 计算之后请注释掉我跑llvm
    //printf_llvm_ir(instruction_list,argv[4],1);


    CheckGlobalVariable(instruction_list);

    //TODO 目前函数内联放在这里了，暂时的
//    printf("=======func inline=========\n");
//    func_inline(instruction_list);
//    printf_llvm_ir(instruction_list,argv[1],1);
//    printf("=======func inline end=======\n");

    //lsy_begin
    fix_array(instruction_list);
    printf_llvm_ir(instruction_list,argv[4],0);
    //lsy_end

    //ljw_begin
//    reg_control(instruction_list,temp);
    //修改all_in_memory开启/关闭寄存器分配
    //ljw_end`1`



    //    ljf_begin
//    如果需要打印到文件里面，打开arm_open_file和arm_close_file,
//    argv[3]里面直接给的就是汇编文件，直接打开就行，修改一下
//
//    arm_open_file(argv[3]);
//    arm_translate_ins(instruction_list,argv[3]);
//    arm_close_file(argv[3]);
    //    ljf_end

    return 0;
}
