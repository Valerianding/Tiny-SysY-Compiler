#include "user.h"

struct _BasicBlock;
struct _Instruction{
    User user;
    
    struct _BasicBlock *Parent;
    //DebugLoc DbgLoc; 
};
typedef struct _Instruction Instruction;


Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2);

//https://llvm.org/doxygen/InstrTypes_8h_source.html
/// Construct a binary instruction, given the opcode and the two
/// operands.  Optionally (if InstBefore is specified) insert the instruction
/// into a BasicBlock right before the specified instruction.  The specified
/// Instruction is allowed to be a dereferenced end iterator.
///
// static BinaryOperator *Create(BinaryOps Op, Value *S1, Value *S2,
//                                 const Twine &Name = Twine(),
//                                 Instruction *InsertBefore = nullptr);


//  BinaryOperator::BinaryOperator(BinaryOps iType, Value *S1, Value *S2,
//                                 Type *Ty, const Twine &Name,
//                                 BasicBlock *InsertAtEnd)
//    : Instruction(Ty, iType,
//                  OperandTraits<BinaryOperator>::op_begin(this),
//                  OperandTraits<BinaryOperator>::operands(this),
//                  InsertAtEnd) {
//    Op<0>() = S1;
//    Op<1>() = S2;
//    setName(Name);
//    AssertOK();
//  }

//template <int Idx, typename U> static Use &OpFrom(const U *that) {
//      return Idx < 0
//        ? OperandTraits<U>::op_end(const_cast<U*>(that))[Idx]
//        : OperandTraits<U>::op_begin(const_cast<U*>(that))[Idx];
//    }
  
//    template <int Idx> Use &Op() {
//      return OpFrom<Idx>(this);
//    }
//    template <int Idx> const Use &Op() const {
//      return OpFrom<Idx>(this);
//    }

// op_iterator       op_begin()       { return getOperandList(); }
//    const_op_iterator op_begin() const { return getOperandList(); }
//    op_iterator       op_end()         {
//      return getOperandList() + NumUserOperands;
//    }

Instruction* ins_new(int op_num);
Instruction *ins_new_binary_operator(int Op, Value *S1, Value *S2);