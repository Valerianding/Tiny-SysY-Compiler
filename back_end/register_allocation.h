#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define SET(i) ( non_available_colors[i/32] |= (1 << (i % 32) ) )
#define CLEAR(i)   ( non_available_colors[i/32] &= ~(1 << (i % 32) ) )
#define CHECK(i)  ( non_available_colors[i/32] & (1 << (i % 32) ) )
#define SETBIT(i,j) (RIG[(((i*rig_num)+j)/32)] |= (1 << ((i*rig_num+j)%32)) ) 
#define CLEARBIT(i,j)   ( RIG[(((i*rig_num)+j)/32)] &= ~(1 << ((i*rig_num+j)%32)) )  
#define CHECKBIT(i,j)  ( RIG[(((i*rig_num)+j)/32)] & (1 << ((i*rig_num+j)%32)) )

#define MAXSTRINGSIZE 40
#define NO_COLOR -1
#define REMOVED -2
#define SPILLED -3
struct queue
{
    int variable_index;
    struct queue *next;
};

struct variable {
	char * name;
	int color;
    int neighbor_count;
};

struct tac_var
{
    char * dest;
    char * left;
    char * right;
};

struct var_fist_last
{
    char * name;
    int first;
    int last;
};

typedef struct numnum *pn;
typedef struct numnum
{
    char * name;
    pn next;
}numnum;

struct  reg_now
{
    int call;
    int exist;
    int op;
    char * name[13];
    char * dest;
    char * left;
    char * right;
    char * param;
};













void init_non_available_colors();
void reset_non_available_colors();
int first_fit_coloring();
int visit(int index);
void __push(int variable_index);
int __pop();
void reset_colors();
void reset_queue();
void color_removed();//需要修改
void print_non_available_colors();
void minimize_RIG();
void test_minimize_RIG();
void print_colors();
void init_RIG();
int supercmp(char *a ,char *b);
//void create_RIG(FILE * re_in);
void create_RIG();
void create_edge(int firstNode,int secondNode);
void create_variable_list();
void print_RIG();
void spill_variable();