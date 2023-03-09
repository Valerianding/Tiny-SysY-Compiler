#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H
#include "value.h"
#include "instruction.h"
#include "sc_list.h"
struct _Function;
struct _BasicBlock;
struct _DomNode;
typedef struct _BasicBlock BasicBlock;
typedef struct _Function Function;
typedef struct _DomNode DomTreeNode;
static int count = 1; //block记数
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


struct _BasicBlock{

    //TODO 我们直接保留HashSet的prevBlocks?
    BlockList prev_blocks;
    struct _BasicBlock *true_block;
    struct _BasicBlock *false_block;
    struct _Function *Parent;
    InstNode *head_node;  // 这个基本块的第一条instruction
    InstNode *tail_node;  // 这个基本块的最后一条instruction
    int visited;
    HashSet *dom; // 记录支配该节点的集合 注意是支配该节点！！
    HashSet *df; // 记录支配边界
    BasicBlock *iDom;
    DomTreeNode *domTreeNode;
    int id;
};

///初始化bblock
void bblock_init(BasicBlock *this);

///将head到tail的InstNode加入this的BasicBlock中
void bb_set_block(BasicBlock *this,InstNode *head,InstNode *tail);

/// 将this加入head
void ins_node_add(InstNode *head,InstNode *this);

///为prev的基本块添加前驱
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

///获取最后一条instNode
InstNode *get_last_inst(InstNode *this);

///根据指令标号搜索instnode
InstNode *search_ins_id(InstNode *head,int id);

///根据label搜索instnode
InstNode *search_ins_label(InstNode *head,int label_id);

///新建一个基本块
BasicBlock *bb_create();

///获得属于trueBlock的后继节点
BlockNode *get_next_block(BlockNode *this);

///获得BlockList的最后一个节点
BasicBlock *blocklist_pop(BlockList list);

///获得下一个前驱节点
BlockNode *get_next_prevblock(BlockNode *this);

///获得所有的前驱节点
HashSet *get_prev_block(BasicBlock *this);

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

///在pos位置后面插入this
void ins_insert_after(InstNode *this,InstNode *pos);

///在pos位置前面插入
void ins_insert_before(InstNode *this, InstNode *pos);

/// 删除某个inst
void delete_inst(InstNode *this);
#endif