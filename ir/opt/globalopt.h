//
// Created by Valerian on 2023/7/3.
//

#ifndef C22V1_GLOBALOPT_H
#define C22V1_GLOBALOPT_H
#include "function.h"
#include "utility.h"


extern Vector *constant_;
//mark all global variables to be constant if only have been read
extern InstNode *instruction_list;
void CheckGlobalVariable(InstNode *list);


#endif //C22V1_GLOBALOPT_H
