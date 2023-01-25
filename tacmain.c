#include <assert.h>
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "stdio.h"
#include "bb_divide.h"
//FIXME: test purpose only!
Symtab* test_symtab;

#include "front_end/travel.h"

extern int yyparse();
extern past TRoot;
Symtab *this;
extern FILE *yyin;
int return_index=0;
int return_stmt_num[10]={0};
char t[5];
insnode_stack S_continue;
insnode_stack S_break;
insnode_stack S_return;
insnode_stack S_and;
insnode_stack S_or;
bool c_b_flag[2]={false,false};

char t_num[3] = {0};
int t_index = 0;

//都还没做初始化那些
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
    //showAst(TRoot,0);

    InstNode *temp = get_next_inst(instruction_list);

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
            print_function_info(parent);
            prevFunction = parent;
        }
        if (cur != prev) {
            print_block_info(cur);
            prev = cur;
        }
    }

    /* 测试dominance的计算 */



    /*
     * a = 1 + 2;
     * b = a + 1;
     */
    /* value * -> get */
    Value *v1 = (Value*)malloc(sizeof(Value));
    value_init_int(v1,1);
    Value *v2 = (Value*)malloc(sizeof(Value));
    value_init_int(v2,2);

    Instruction *ins1 = ins_new_binary_operator(Add,v1,v2);
    Value *dest1 = ins_get_value(ins1);
    int SIZE = 10;
    dest1->name = (char*)malloc(sizeof(char) * SIZE);
    dest1->name = "a";

    /* 也可以是：
     * Instruction *insw = ins_new_binary_operator(Add,&ins1->user.value,v1)
     * */
    Instruction *ins2 = ins_new_binary_operator(Add,(Value*)ins1,v1);
    Value *dest2 = ins_get_value(ins2);
    dest2->name = (char*)malloc(sizeof(char) * SIZE);
    dest2->name = "b";

    InstNode *instNode1 = new_inst_node(ins1);
    InstNode *instNode2 = new_inst_node(ins2);
    print_one_ins_info(instNode1);
    print_one_ins_info(instNode2);

    return 0;
}
