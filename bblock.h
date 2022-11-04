
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
struct _BasicBlock
{
    Value value;
    struct _Function *Parent;
    struct sc_list inst_list;
};

typedef struct _InstNode InstNode;
struct _InstNode{
    Instruction* inst;
    struct sc_list list;
};

void bblock_init(BasicBlock* this, Function* func);

void bblock_add_inst_back(BasicBlock* this, Instruction* inst);

Instruction* bblock_pop_inst_back(BasicBlock* this);


#endif