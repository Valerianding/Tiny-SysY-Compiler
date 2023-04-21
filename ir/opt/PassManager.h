//
// Created by Valerian on 2023/4/15.
//

#ifndef C22V1_PASSMANAGER_H
#define C22V1_PASSMANAGER_H
#include "ConstFolding.h"
#include "DeadCodeElimination.h"
#include "CommonSubexpressionElimination.h"
#include "livenessanalysis.h"
#include "Loop.h"
void RunPasses(Function *currentFunction);
#endif //C22V1_PASSMANAGER_H
