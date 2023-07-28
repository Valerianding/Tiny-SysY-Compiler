//
// Created by 12167 on 2023/7/19.
//

#ifndef C22V1_TREE_BALANCING_H
#define C22V1_TREE_BALANCING_H

#include "ast.h"
#include "past_stack.h"
#include "value_stack.h"
#include "queue.h"
#include "hash_set.h"
void balance(past *root, HashMap* rank_map, HashSet* root_set);
void tree_balancing(past *expr);
#endif //C22V1_TREE_BALANCING_H
