//
// Created by Valerian on 2023/6/30.
//

#ifndef C22V1_LOOPREDUCE_H
#define C22V1_LOOPREDUCE_H
#include "function.h"
#include "Loop.h"
#include "utility.h"
bool RunOnLoop(Loop *loop){
    //need to dfs travel


}

bool CheckLoop(Loop *loop){

}

bool LoopStrengthReduction(Function *function){
    //
    bool modified = false;
    HashSet *loops = function->loops;
    HashSetFirst(loops);
    for(Loop *loop = HashSetNext(function->loops); loop != NULL; loop = HashSetNext(function->loops)){
        //
       // RunOnLoop()
    }
}


#endif //C22V1_LOOPREDUCE_H
