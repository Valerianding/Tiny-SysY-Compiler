
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
typedef struct _InstNode InstNode;
struct _InstNode{
    Instruction* inst;
    struct sc_list list;
};

typedef InstNode* InstList;

/* 可能用不到 */
typedef struct _BlockNode BlockNode;
struct _BlockNode{
    BasicBlock *block;
    struct sc_list list;
};
typedef BlockNode* BlockList;

struct _BasicBlock
{
    Value value;
    struct _Function *Parent;
    InstList *inst_list; //链表
    BlockList *block_list;  //感觉不如直接设计一个prev 和 next？？
};

void bblock_init(BasicBlock* this, Function* func);

void bblock_add_inst_back(BasicBlock* this, Instruction* inst);

Instruction* bblock_pop_inst_back(BasicBlock* this);

Function *bblock_get_parent(BasicBlock* this);

/// Unlink 'this' from the containing function, but do not delete it.
void removeFromParent();

/// Unlink this basic block from its current function and insert it into
/// the function that \p MovePos lives in, right before \p MovePos.
void moveBefore(BasicBlock *MovePos);

/// Unlink this basic block from its current function and insert it
/// right after \p MovePos in the function \p MovePos lives in.
void moveAfter(BasicBlock *MovePos);

/// Insert unlinked basic block into a function.
///
/// Inserts an unlinked basic block into \c Parent.  If \c InsertBefore is
/// provided, inserts before that basic block, otherwise inserts at the end.
///
/// \pre \a getParent() is \c nullptr.
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
///
/// This function ensures all instructions AFTER and including the specified
/// iterator \p I are part of the original basic block. All Instructions
/// BEFORE the iterator \p I are moved to the new BB and an unconditional
/// branch is added to the new BB. The new basic block is returned.
///
/// Note that this only works on well formed basic blocks (must have a
/// terminator), and \p 'I' must not be the end of instruction list (which
/// would cause a degenerate basic block to be formed, having a terminator
/// inside of the basic block).  \p 'I' cannot be a iterator for a PHINode
/// with multiple incoming blocks.
///
/// Also note that this doesn't preserve any passes. To split blocks while
/// keeping loop information consistent, use the SplitBlockBefore utility
/// function.
// BasicBlock *splitBasicBlockBefore(iterator I, const Twine &BBName = "");




#endif