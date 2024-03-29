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
#include "fix_array.h"
#include "line_scan.h"
#include "graph_color.h"
#define ALL 1
extern FILE *yyin;
extern HashMap *callGraph;
extern HashSet *visitedCall;
extern int yylex();
extern int yyparse();
//extern past TRoot;
Symtab *this;

int return_index=0;
int return_stmt_num[20]={0};
char t[8];
int t_index = 0;
insnode_stack S_continue;
insnode_stack S_break;
insnode_stack S_return;
insnode_stack S_and;
insnode_stack S_or;
past_stack ps_logic;

char t_num[7] = {0};
int flag_blocklist;
HashMap *tokenMap;
int lexOnly;
int def_call;

int START_LINE = 1;
int STOP_LINE = 1;

struct _InstNode *instruction_list;

void yyerror(char *s)
{
    printf("%s\n", s);
}

bool Optimize = false;

int main(int argc, char* argv[]){
    //assert(sizeof(unsigned int) == 4);
    //lsy
    if(argc < 2 ){
        printf("ERROR: input file name is needed. \n");
        exit(0);
    }

    //看看是否开启优化
    if(argc == 6){
        Optimize = true;
    }

    yyin=fopen(argv[4], "r");

    tokenMap = HashMapInit();

    Instruction *ins_head= ins_new_unary_operator(ALLBEGIN,NULL);
    instruction_list= new_inst_node(ins_head);
    t[0]='%';
    //TODO 还有没有更好的做法呢
    init_insnode_stack(&S_continue);
    init_insnode_stack(&S_break);
    init_insnode_stack(&S_return);
    init_insnode_stack(&S_and);
    init_insnode_stack(&S_or);
    init_past_stack(&ps_logic);
    this=(Symtab*) malloc(sizeof(Symtab));
    symtab_init(this);
    //先跑一遍词法分析
    lexOnly = 1;
    def_call = 0;
    int token;
    while(token = yylex());
    //重置输入流
    fclose(yyin);
    yyin=fopen(argv[4], "r");
    lexOnly = 0;
    //语法分析
    yyparse();

    return_index=0;
    TRoot=TRoot->left;
    stack_new(this);
    declare_global_alloca(this->value_maps->next);
    flag_blocklist=1;
    create_instruction_list(TRoot,NULL,0);
    travel_finish_type(instruction_list);
    move_give_param(instruction_list);
    test_wrong_ret(instruction_list);
//  printf_llvm_ir(instruction_list,argv[4],1);
//  print_array(instruction_list);
//  showAst(TRoot,0);

    //init CallGraph
    callGraph = HashMapInit();
    visitedCall = HashSetInit();

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
    global2local(instruction_list);


    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        calculateNonLocals(currentFunction);

        mem2reg(currentFunction);

        //Loop invariant code motion 需要使用live-out信息
        calculateLiveness(currentFunction);
    }


    CheckGlobalVariable(instruction_list);
    JudgeXor(instruction_list);
    combineZext(instruction_list);

    bool NOTOK = containFloat(instruction_list);
    NOTOK = false;

    NOTOK=false;
    //构建Function
    Function *start = ReconstructFunction(instruction_list);

    //先跑一次
    //如果要fuc inline一定要dom一下
    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next) {
        if(!NOTOK){
            RunBasicPasses(currentFunction);
            buildCallGraphNode(currentFunction);
        }
    }

    if(Optimize && !NOTOK) {
        for (Function *currentFunction = start;
             currentFunction != NULL; currentFunction = currentFunction->Next) {
            // issimple(currentFunction);
            sideEffectAnalysis(currentFunction);
            RunOptimizePasses(currentFunction);
            RedundantCallElimination(currentFunction);
            renameVariables(currentFunction);
        }
    }
    //简单递归的消除
    if(Optimize && !NOTOK) {
        for (Function *currentFunction = start;
             currentFunction != NULL; currentFunction = currentFunction->Next) {
            // issimple(currentFunction);
            renameVariables(currentFunction);
        }

        //重新构建Function
        start = ReconstructFunction(instruction_list);

        global2local(instruction_list);

        for (Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next) {

            dominanceAnalysis(currentFunction);

            calculateNonLocals(currentFunction);

            mem2reg(currentFunction);

            //Loop invariant code motion 需要使用live-out信息
            calculateLiveness(currentFunction);
        }
    }

    printf_llvm_ir(instruction_list,argv[4],1);

    if(Optimize) {
        func_inline(instruction_list, 124);

        //重新构建Function
        start = ReconstructFunction(instruction_list);

        global2local(instruction_list);

        for (Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next) {

            dominanceAnalysis(currentFunction);

            calculateNonLocals(currentFunction);

            mem2reg(currentFunction);

            //Loop invariant code motion 需要使用live-out信息
            calculateLiveness(currentFunction);
        }
    }

    printf_llvm_ir(instruction_list,argv[4],1);

    if(!NOTOK && Optimize){
        for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next) {
            RunOptimizePasses(currentFunction);
            RunOptimizePasses(currentFunction);
        }

        printf_llvm_ir(instruction_list,argv[4],1);

        for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
            loopAnalysis(currentFunction);
            CalcOnLoop(currentFunction);
            removeUselessLocalArray(currentFunction);
        }
    }

    printf_llvm_ir(instruction_list,argv[4],1);

#if ALL
    //phi上的优化
    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
        SSADeconstruction(currentFunction);
        renameVariables(currentFunction);
        cleanLiveSet(currentFunction);
    }

    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
        //printf("function: %s\n",currentFunction->entry->head_node->inst->user.use_list[0].Val->name);
        clear_visited_flag(currentFunction->entry);
        //printf("after out of SSA!\n");
        calculateLiveness(currentFunction);
        printLiveness(currentFunction);
    }

    //  printf_llvm_ir(instruction_list,argv[4],0);

    for(Function *currentFunction = start;
        currentFunction != NULL; currentFunction = currentFunction->Next) {
        dominanceAnalysis(currentFunction);
        topCfg(currentFunction);
    }


    fix_array(instruction_list);



//    图着色
    reg_alloca_(start);
//    线性扫描
    line_scan(instruction_list,start);
//    reg_control(instruction_list,start);
//    gcp_allocate(instruction_list,start);
    //修改all_in_memory开启/关闭寄存器分配

    arm_open_file(argv[3]);
    arm_translate_ins(instruction_list,argv[3]);
    arm_close_file();
#endif
    return 0;
}