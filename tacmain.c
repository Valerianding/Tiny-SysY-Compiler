#include <assert.h>
#include "./back_end/arm.h"
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "stdio.h"
#include "bb_divide.h"
#include "dominance.h"
#include "mem2reg.h"
#include "front_end/travel.h"
//FIXME: test purpose only!
Symtab* test_symtab;

void test(HashSet *set){
    int *a = (int*)malloc(sizeof(int));
    set = HashSetInit();
    HashSetAdd(set,(void*)a);
}

extern int yyparse();
extern past TRoot;
Symtab *this;
extern FILE *yyin;
int return_index=0;
int return_stmt_num[10]={0};
char t[5];
int t_index = 0;
insnode_stack S_continue;
insnode_stack S_break;
insnode_stack S_return;
insnode_stack S_and;
insnode_stack S_or;
bool c_b_flag[2]={false,false};

char t_num[3] = {0};

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
    create_instruction_list(TRoot,NULL);
    printf_llvm_ir(instruction_list,argv[1]);
//    fix_array(instruction_list);
    //print_array(instruction_list);
    //showAst(TRoot,0);

    InstNode *temp = get_next_inst(instruction_list);
    InstNode *temp2 = instruction_list;

    //丁老师
    bblock_divide(instruction_list);

    /* 测试所有instruction list */
    for(;instruction_list != NULL;instruction_list = get_next_inst(instruction_list)){
        print_one_ins_info(instruction_list);
    }

    printf("--------------\n");

    /* 测试所有BasicBlock的连接 以及Function的连接 */
    BasicBlock *prev = nullptr;
    Function *prevFunction = nullptr;
    for(;temp != NULL;temp = get_next_inst(temp)) {
        BasicBlock *cur = temp->inst->Parent;
        Function *parent = cur->Parent;
        if(parent != prevFunction){
            /* 测试dominance的计算 */
            printf("-------function  start---------\n");
            print_function_info(parent);
            calculate_dominance(parent);
            calculate_dominance_frontier(parent);
            calculate_iDominator(parent);
            calculate_DomTree(parent);
            mem2reg(parent);

            prevFunction = parent;
            printf("------after a function------\n");
        }
        if (cur != prev) {
            print_block_info(cur);
            prev = cur;
        }
    }

    // mem2reg 之后的
    printf_llvm_ir(temp2,argv[1]);
    //    ljf
    arm_translate_ins(temp2);
    return 0;
}
