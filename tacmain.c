#include <assert.h>
#include "value.h"
#include "use.h"
#include "user.h"
#include "instruction.h"
#include "symtab.h"
#include "bblock.h"
#include "function.h"
#include "stdio.h"

//FIXME: test purpose only!
Symtab* test_symtab;


#include <stdio.h>
#include "symtab.h"
#include "bblock.h"
#include "front_end/travel.h"

extern int yyparse();
extern past TRoot;
Symtab *this;
extern FILE *yyin;
int return_index=0;
int return_stmt_num[10]={0};
char t[5];

//都还没做初始化那些
struct _InstNode *instruction_list;

void yyerror(char *s)
{
    printf("%s\n", s);
}

int main(int argc, char* argv[]){
    if(argc < 2 ){
        printf("ERROR: input file name is needed. \n");
        exit(0);
    }
    yyin=fopen(argv[1], "r");

    Instruction *ins_head= ins_new_unary_operator(ALLBEGIN,NULL);
    instruction_list= new_inst_node(ins_head);
    t[0]='%';

    this=(Symtab*) malloc(sizeof(Symtab));
    symtab_init(this);
    yyparse();
    TRoot=TRoot->left;
    stack_new(this);
    create_instruction_list(TRoot);
    printf_llvm_ir(instruction_list);
    //showAst(TRoot,0);
    bblock_divide(instruction_list);
    int i = 0;
    for(;instruction_list != NULL;instruction_list = get_next_inst(instruction_list)){
        printf("%d : opcode:",i++);
        print_ins_opcode(instruction_list->inst);
        if(instruction_list->inst->Opcode == br_i1){
            printf("%d %d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location,instruction_list->inst->user.value.pdata->instruction_pdata.false_goto_location);
        }else if(instruction_list->inst->Opcode == br){
            printf("%d",instruction_list->inst->user.value.pdata->instruction_pdata.true_goto_location);
        }else if(instruction_list->inst->Opcode == Label){
            printf("%d",instruction_list->inst->i);
        }
        if(instruction_list->inst->Parent != NULL){
            printf(" parent:%p",instruction_list->inst->Parent);
        }else{
            printf(" parent:NULL");
        }
        printf("\n");
    }
    return 0;
}
