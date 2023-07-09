//
// Created by Valerian on 2023/5/25.
//

#include "other.h"

//other optimizations
//which means only happens in corner case

//case1
//remove the function
void RemoveUselessFunction(InstNode *insHead){
    //first go through all the function we have
}


//case2
//remove those useless loop
void removeUselessLoop(Function* currentFunction){
    //从外层循环开始
    HashSet *loops = currentFunction->loops;
    HashSetFirst(loops);
    for(Loop *root = HashSetNext(loops); root != NULL; root = HashSetNext(loops)){
        remove
    }
}

//bool removeOneLoop(Loop *loop){
//    Value *initValue = loop ->initValue;
//    Value *endCondition = loop->end_cond;
//    Instruction *icmp = (Instruction *)endCondition;
//
//    //see if the rhs is a constant
//    Value *lhs = ins_get_lhs(icmp);
//    Value *rhs = ins_get_rhs(icmp);
//    Value *compare = (lhs == initValue ? rhs : lhs);
//
//    bool useless = false;
//    // 0 > 10
//    //TODO :全部转换成float进行比较是否正确呢？
//    if(isImm(compare) && isImm(initValue)){
//        float init = 0;
//        float cmp = 0;
//        switch (compare->VTy->ID) {
//            case Int:{
//                cmp = (float)compare->pdata->var_pdata.iVal;
//                break;
//            }
//            case Float:{
//                cmp = compare->pdata->var_pdata.fVal;
//                break;
//            }
//            default:
//                assert(false);
//        }
//
//        switch (initValue->VTy->ID) {
//            case Int:{
//
//            }
//        }
//        switch (icmp->Opcode) {
//            case LESS:{
//                break;
//            }
//            case LESSEQ:{
//
//                break;
//            }
//            default:
//                assert(false);
//        }
//    }
//    HashSetFirst(loop->child);
//    for(Loop *subLoop = HashSetNext(loop->child); subLoop != NULL; subLoop = HashSetNext(loop->child)){
//        removeOneLoop(subLoop);
//    }
//}