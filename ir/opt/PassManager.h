//
// Created by Valerian on 2023/4/15.
//

#ifndef C22V1_PASSMANAGER_H
#define C22V1_PASSMANAGER_H
#include "constfolding.h"
#include "dce.h"
#include "cse.h"
#include "livenessanalysis.h"
#include "loopinfo.h"
#include "dvnt.h"
#include "memlvn.h"
#include "licm.h"
#include "globalopt.h"
#include "sideeffect.h"
#include "inscomb.h"
#include "other.h"
#include "loopconv.h"
#include "gcm.h"
void RunBasicPasses(Function *currentFunction);
void RunOptimizePasses(Function *currentFunction);
#endif //C22V1_PASSMANAGER_H
