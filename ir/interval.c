#include "interval.h"


void interval_init(Interval* this){

    this->head_node = NULL;
    sc_array_init(&this->nodes);
    sc_array_init(&this->successors);
    sc_array_init(&this->predecessors);

}

/// contains - Find out if a basic block is in this interval
bool interval_contains(Interval* this, BasicBlock *BB) {
    void *it;
    sc_array_foreach (&this->nodes, it) {
		if(it == BB)
            return true;
	}
    return false;
}

/// isSuccessor - find out if a basic block is a successor of this Interval
bool interval_successor(Interval* this, BasicBlock *BB){
    void *it;
    sc_array_foreach (&this->successors, it) {
		if(it == BB)
            return true;
	}
    return false;
}