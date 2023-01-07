
/// LLVM Basic Block Representation
///
/// This represents a single basic block in LLVM. A basic block is simply a
/// container of instructions that execute sequentially. Basic blocks are Values
/// because they are referenced by instructions such as branches and switch
/// tables. The type of a BasicBlock is "Type::LabelTy" because the basic block
/// represents a label to which a branch can jump.
///
/// A well formed basic block is formed of a list of non-terminating
/// instructions followed by a single terminator instruction. Terminator
/// instructions may not occur in the middle of basic blocks, and must terminate
/// the blocks. The BasicBlock class allows malformed basic blocks to occur
/// because it may be useful in the intermediate stage of constructing or
/// modifying a program. However, the verifier will ensure that basic blocks are
/// "well formed".

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
    Instruction* inst;
    struct sc_list list;
} InstNode;

typedef struct _BasicBlock
{
    User user;
    struct _BasicBlock *next;
    struct _BasicBlock *jump;
    struct _Function *Parent;
    InstNode *head_node;  // 这个基本块的第一条instruction
    InstNode *tail_node;  // 这个基本块的最后一条instruction
    int flag;
    HashMap *in;
    HashMap *out;
}BasicBlock;

///初始化bblock
void bblock_init(BasicBlock *this, Function *func);

///将inst加入到bblock的inst链表的最后
void bblock_add_inst_back(BasicBlock *this, Instruction *inst);

///将bblock的最后的inst剔除
void bblock_pop_inst_back(BasicBlock *this);

///获得bblock的最后的inst
InstNode* bblock_get_inst_back(BasicBlock *this);

///获得bblock的头inst
InstNode* bblock_get_inst_head(BasicBlock *this);

///获得bblock的所在的function
Function *bblock_get_parent(BasicBlock *this);

/// Unlink 'this' from the containing function, but do not delete it.
void removeFromParent(BasicBlock *this);

/// Unlink this basic block from its current function and insert it into
/// the function that \p MovePos lives in, right before \p MovePos.
/// 现在是不涉及基本块搭建之后修改的 也就是说只是addbefore
void moveBefore(BasicBlock *this,BasicBlock *MovePos);

/// Unlink this basic block from its current function and insert it
/// right after \p MovePos in the function \p MovePos lives in.
void moveAfter(BasicBlock *this,BasicBlock *MovePos);

/// Insert unlinked basic block into a function.
///
/// Inserts an unlinked basic block into \c Parent.  If \c InsertBefore is
/// provided, inserts before that basic block, otherwise inserts at the end.
///
void insertInto(Function *Parent, BasicBlock *InsertBefore);

/// Return the predecessor of this block if it has a single predecessor
/// block. Otherwise return a null pointer.
BasicBlock *getSinglePredecessor();


/// Return the predecessor of this block if it has a unique predecessor
/// block. Otherwise return a null pointer.
///
/// Note that unique predecessor doesn't mean single edge, there can be
/// multiple edges from the unique predecessor to this block (for example a
/// switch statement with multiple cases having the same destination).
BasicBlock *getUniquePredecessor();

/// Return true if this block has exactly N predecessors.
bool hasNPredecessors(unsigned N);

/// Return true if this block has N predecessors or more.
bool hasNPredecessorsOrMore(unsigned N);

/// Return the successor of this block if it has a single successor.
/// Otherwise return a null pointer.
///
/// This method is analogous to getSinglePredecessor above.
BasicBlock *getSingleSuccessor();

/// Return the successor of this block if it has a unique successor.
/// Otherwise return a null pointer.
///
/// This method is analogous to getUniquePredecessor above.
const BasicBlock *getUniqueSuccessor();


/// Print the basic block to an output stream with an optional
/// AssemblyAnnotationWriter.
// void print(raw_ostream &OS, AssemblyAnnotationWriter *AAW = nullptr,
//             bool ShouldPreserveUseListOrder = false,
//             bool IsForDebug = false);

/// Split the basic block into two basic blocks at the specified instruction
/// and insert the new basic blocks as the predecessor of the current block.

/// 创建一个InstNode 注意Instruction是一定自己分配了内存的
InstNode* new_inst_node(Instruction* inst);

///
InstNode *get_prev_inst(InstNode *this);

///
InstNode *get_next_inst(InstNode *this);

///
InstNode *search_inst_node(InstNode *head,int id);

///划分基本块
void bblock_divide(InstNode *head);

///将head到tail的InstNode加入this的BasicBlock中
void bb_set_block(BasicBlock *this,InstNode *head,InstNode *tail);

///计算BasicBlock里的指令数目
size_t bb_count_ins(BasicBlock *this);

/// 将this加入head
void ins_node_add(InstNode *head,InstNode *this);

///获得下一个BasicBlock
BasicBlock *get_next_block(BasicBlock *this);

///获得上一个BasicBlock
BasicBlock *get_prev_block(BasicBlock *this);
#endif