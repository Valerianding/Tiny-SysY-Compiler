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

#define ALL 0
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
    assert(sizeof(unsigned int) == 4);
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
//    printf_llvm_ir(instruction_list,argv[4],1);

    for(Function *currentFunction = block->Parent; currentFunction != NULL; currentFunction = currentFunction->Next){
        calculateNonLocals(currentFunction);

        mem2reg(currentFunction);

        //Loop invariant code motion 需要使用live-out信息
        calculateLiveness(currentFunction);

        printLiveness(currentFunction);
    }

//    //mem2reg之后，优化前
//    printf_llvm_ir(instruction_list,argv[4],1);


    CheckGlobalVariable(instruction_list);
    JudgeXor(instruction_list);
    combineZext(instruction_list);


    //func_inline(instruction_list,255);


    //重新构建Function
    Function *start = ReconstructFunction(instruction_list);

    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
        //这里build CallGraphNode 需要在内联之后进行callgraph的
        buildCallGraphNode(currentFunction);
    }

    //OK 现在开始我们不会对
    printf_llvm_ir(instruction_list,argv[4],1);


    //先跑一次
    //cse cf
    //如果要fuc inline一定要dom一下
    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next) {
        //RunBasicPasses(currentFunction);
        loop(currentFunction);
        dominanceAnalysis(currentFunction);
        markDominanceDepth(currentFunction);
        markLoopNest(currentFunction);
        ScheduleEarly(currentFunction);
    }

//    if(Optimize) {
//        travel();
//        for (Function *currentFunction = start;
//             currentFunction != NULL; currentFunction = currentFunction->Next) {
//            dominanceAnalysis(currentFunction);
//             RunOptimizePasses(currentFunction);
//        }
//
//    }

    printf_llvm_ir(instruction_list,argv[4],0);

    for (Function *currentFunction = start;
         currentFunction != NULL; currentFunction = currentFunction->Next) {
        DVNT(currentFunction);
    }

    printf_llvm_ir(instruction_list,argv[4],0);

    for (Function *currentFunction = start;
         currentFunction != NULL; currentFunction = currentFunction->Next) {
        clearInsVisited(currentFunction);
        ScheduleLate(currentFunction);
        renameVariables(currentFunction);
    }
    printf_llvm_ir(instruction_list,argv[4],0);
#if ALL
    //phi上的优化
    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
        SSADeconstruction(currentFunction);
        renameVariables(currentFunction);
        cleanLiveSet(currentFunction);
    }

    //请注释掉我跑llvm脚本 phi函数消除
    printf_llvm_ir(instruction_list,argv[4],1);

//
    for(Function *currentFunction = start; currentFunction != NULL; currentFunction = currentFunction->Next){
        printf("function: %s\n",currentFunction->entry->head_node->inst->user.use_list[0].Val->name);
        clear_visited_flag(currentFunction->entry);
        printf("after out of SSA!\n");
        calculateLiveness(currentFunction);
        printLiveness(currentFunction);
    }

    // Liveness 计算之后请注释掉我跑llvm
//    printf_llvm_ir(instruction_list,argv[4],1);


    //lsy_begin
//    printf("=================fix===================\n");
    fix_array(instruction_list);
//    printf_llvm_ir(instruction_list,argv[4],0);
    //lsy_end

    //ljw_begin
    reg_control(instruction_list,start);
    //修改all_in_memory开启/关闭寄存器分配
    //ljw_end`1`

    //    ljf_begin
//    如果需要打印到文件里面，打开arm_open_file和arm_close_file,
//    argv[3]里面直接给的就是汇编文件，直接打开就行，修改一下

//    arm_open_file(argv[3]);
//    arm_translate_ins(instruction_list,argv[3]);
//    arm_close_file(argv[3]);
    //    ljf_end

#endif
    return 0;
}