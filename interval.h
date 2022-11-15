#ifndef INTERVAL_H
#define INTERVAL_H
#include "bblock.h"
#include "sc_array.h"

//===----------------------------------------------------------------------===//
//
// https://llvm.org/doxygen/Interval_8h_source.html
/// Interval Class - An Interval is a set of nodes defined such that every node
/// in the interval has all of its predecessors in the interval (except for the
/// header)
struct _Interval
{
    /// HeaderNode - The header BasicBlock, which dominates all BasicBlocks in this
    /// interval.  Also, any loops in this interval must go through the HeaderNode.
    BasicBlock *head_node;

    /// Nodes - The basic blocks in this interval.
    // std::vector<BasicBlock*> Nodes;
    struct sc_array_ptr nodes;
    
  
    /// Successors - List of BasicBlocks that are reachable directly from nodes in
    /// this interval, but are not in the interval themselves.
    /// These nodes necessarily must be header nodes for other intervals.
    // std::vector<BasicBlock*> Successors;
    struct sc_array_ptr successors;
  
    /// Predecessors - List of BasicBlocks that have this Interval's header block
    /// as one of their successors.
    // std::vector<BasicBlock*> Predecessors;
    struct sc_array_ptr predecessors;

};

typedef struct _Interval Interval;

void interval_init(Interval* this);
bool interval_contains(Interval* this, BasicBlock *BB);
bool interval_successor(Interval* this, BasicBlock *BB);
#endif

