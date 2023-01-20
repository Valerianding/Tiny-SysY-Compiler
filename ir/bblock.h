#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H
#include "value.h"
#include "instruction.h"
#include "sc_list.h"

struct _Function;
typedef struct _Function Function;
typedef struct _BasicBlock BasicBlock;

/* 这里的设计结构 */
typedef struct _InstNode{
    Instruction *inst;
    struct sc_list list;
}InstNode;

typedef struct _BlockNode{
    BasicBlock *block;
    struct sc_list list;
}BlockNode;

typedef BlockNode * BlockList;

typedef struct _BasicBlock
{
    BlockList prev_blocks;
    struct _BasicBlock *true_block;
    struct _BasicBlock *false_block;
    struct _Function *Parent;
    InstNode *head_node;  // 这个基本块的第一条instruction
    InstNode *tail_node;  // 这个基本块的最后一条instruction
    int visited;
    HashMap *in;
    HashMap *out;
}BasicBlock;

///初始化bblock
void bblock_init(BasicBlock *this);

///将head到tail的InstNode加入this的BasicBlock中
void bb_set_block(BasicBlock *this,InstNode *head,InstNode *tail);

/// 将this加入head
void ins_node_add(InstNode *head,InstNode *this);

void bb_add_prev(BasicBlock *prev,BasicBlock *pos);

///获得bblock的最后的inst
InstNode* bblock_get_inst_back(BasicBlock *this);

///获得bblock的头inst
InstNode* bblock_get_inst_head(BasicBlock *this);

/// 创建一个InstNode 注意Instruction是一定自己分配了内存的
InstNode* new_inst_node(Instruction* inst);

///通过this获得下一个instnode
InstNode *get_prev_inst(InstNode *this);

///通过this获得下一个的instnode
InstNode *get_next_inst(InstNode *this);

//lsy
InstNode *get_last_inst(InstNode *this);

///根据指令标号搜索instnode
InstNode *search_ins_id(InstNode *head,int id);

///根据label搜索instnode
InstNode *search_ins_label(InstNode *head,int label_id);

///新建一个基本块
BasicBlock *bb_create();

///获得属于trueBlock的后继节点
BasicBlock *get_next_block(BasicBlock *this);

///获得下一个前驱节点
BlockNode *get_next_prevblock(BlockNode *this);

///获得所有的前驱节点
BlockList get_prev_block(BasicBlock *this);

///获得bblock的所在的function
Function *bblock_get_parent(BasicBlock *this);

///计算BasicBlock里的指令数目
size_t bb_count_ins(BasicBlock *this);

///打印一条语句的信息
void print_one_ins_info(InstNode *instruction_list);

///打印基本块的info
void print_block_info(BasicBlock *this);

///清除所有的block的访问标记
void clear_visited_flag(InstNode *head);

///加入到blocklist当中，仅用于dominance信息的计算
void add_blocklist(BlockList list,BasicBlock *block);

/// Unlink 'this' from the containing function, but do not delete it.
void removeFromParent(BasicBlock *this);


void moveBefore(BasicBlock *this,BasicBlock *MovePos);


void moveAfter(BasicBlock *this,BasicBlock *MovePos);


void insertInto(Function *Parent, BasicBlock *InsertBefore);

/// Return the predecessor of this block if it has a single predecessor
/// block. Otherwise return a null`  pointer.
BasicBlock *getSinglePredecessor();
#endif