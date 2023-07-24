#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../ir/instruction.h"
#include "../ir/bblock.h"
#include "../ir/opt/utility.h"
#include "value.h"
#include "travel.h"
#include "type.h"
#include "user.h"
#include "function.h"
#include "offset.h"
#define SET(i) ( non_available_colors[i/32] |= (1 << (i % 32) ) )
#define CLEAR(i)   ( non_available_colors[i/32] &= ~(1 << (i % 32) ) )
#define CHECK(i)  ( non_available_colors[i/32] & (1 << (i % 32) ) )
#define SETBIT(i,j) (RIG[(((i*rig_num)+j)/32)] |= (1 << ((i*rig_num+j)%32)) ) 
#define CLEARBIT(i,j)   ( RIG[(((i*rig_num)+j)/32)] &= ~(1 << ((i*rig_num+j)%32)) )  
#define CHECKBIT(i,j)  ( RIG[(((i*rig_num)+j)/32)] & (1 << ((i*rig_num+j)%32)) )

#define MAXSTRINGSIZE 100
#define NO_COLOR -1
#define REMOVED -2
#define SPILLED -3


#define all_in_memory 0
#define reg_alloc_test 0
struct reg_queue
{
    int variable_index;
    struct reg_queue *next;
};

struct SString {
	char * name;
};

struct variable {
	char * name;
	int color;
    int neighbor_count;
};

struct name_num
{
    int num;
    char *name;
    int ifparam;
    int iffuc;
    int first_use;
    int first;
    int last;
    int last_def;
    int first_is_use;
    int last_is_use;
    int isin;
    int isout;
    int ys;
    int kua;
};

struct BLOCK_list
{
    BasicBlock * reg_block;
};


struct  reg_edge
{
    int a;
    int b;
};
struct  reg_now
{
    int node_id;
    Instruction * irnode;
    char * dest_name;
    char * left_name;
    char * right_name;
    int dest_use;//0为def，1为use
    int left_use;
    int right_use;
    int dest_first;
    int left_first;
    int right_first;
    int give_param;
    int reg_3[3];
    int whoinreg[13];
    int jumpto[2];
    int jumptoid[2];
    int from_cnt;
    int from[100];
    int label;
    int visited;
};




void init_non_available_colors();
void reset_non_available_colors();
int first_fit_coloring();
int visit(int index);
void reg__push(int variable_index);
int reg__pop();
void reset_colors();
void reset_queue();
void color_removed();//需要修改
void print_non_available_colors();
void minimize_RIG();
void test_minimize_RIG();
void print_colors();
void init_RIG();
int supercmp(char *a ,char *b);
int find_var(char * str);
int find_var_global(char * str);
//void create_RIG(FILE * re_in);
void create_RIG();
void jumpfromto();
int check_edge();//检查rig
void create_edge(int firstNode,int secondNode);
void create_variable_list();
void print_RIG();
void spill_variable();
void printf_llvm_ir_withreg(struct _InstNode *instruction_node);
void reg_control_func(Function *currentFunction);
void reg_control_block(BasicBlock *cur);
void reg_control_block_temp(BasicBlock *cur);
int is_func_param(char * str);
void live_init_block();
void visittac();
void fyreg(int i,int j);
void init_global(BasicBlock *cur);
void add_to_global();
char * find_live_name(int id);
void reg_inmem_one_ins(int id); // 放到内存中
int use_type(struct _InstNode *temp);
int is_Immediate(int type_id);
void end_reg();//未实现
void test_ans();
void travel_ir(InstNode *instruction_node);//部分ir浮点数未考虑
void addtolive(char * name,int tacid,int ifuse,int paramid);
void addtoin(BasicBlock *this_block);
void addtoout(BasicBlock *this_block);
void create_bian(int i,int j);
void bian_init(BasicBlock * this_block);
void bian_init_test(BasicBlock * this_block);
void add_to_ir();
void add_to_echo();
void echo_to_ir();
void ir_reg_init(InstNode *instruction_node);
void clean_reg();//完成后释放内存
void clean_reg_global();
void addinout();
void printf_asm_test(char * filename_test);
void reg_control(struct _InstNode *instruction_node,Function *start);