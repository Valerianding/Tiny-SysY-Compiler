#ifndef C22V1_RM
#define C22V1_RM
#include "utility.h"
#include "function.h"
#include "inscomb.h"
#include "constfolding.h"
typedef struct recursive_node{
    int result;
    int num;
}r_node;
int issimple(Function * tempFunction);
void remake_func(Function * function);

#endif 