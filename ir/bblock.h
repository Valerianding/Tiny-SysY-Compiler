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
    int flag;
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

///
InstNode *get_prev_inst(InstNode *this);

///
InstNode *get_next_inst(InstNode *this);

//lsy
InstNode *get_last_inst(InstNode *this);

///
InstNode *search_ins_id(InstNode *head,int id);

///
InstNode *search_ins_label(InstNode *head,int label_id);

BasicBlock *bb_create();

///获得属于trueBlock的后继节点
BasicBlock *get_next_block(BasicBlock *this);

///获得所有的前驱节点
BlockList get_prev_block(BasicBlock *this);

///获得bblock的所在的function
Function *bblock_get_parent(BasicBlock *this);

///计算BasicBlock里的指令数目
size_t bb_count_ins(BasicBlock *this);


/// Unlink 'this' from the containing function, but do not delete it.
void removeFromParent(BasicBlock *this);


void moveBefore(BasicBlock *this,BasicBlock *MovePos);


void moveAfter(BasicBlock *this,BasicBlock *MovePos);


void insertInto(Function *Parent, BasicBlock *InsertBefore);

/// Return the predecessor of this block if it has a single predecessor
/// block. Otherwise return a null`  pointer.
BasicBlock *getSinglePredecessor();
#endif