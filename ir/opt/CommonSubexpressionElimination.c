//
// Created by Valerian on 2023/4/16.
//

#include "CommonSubexpressionElimination.h"

//TODO 解决全局的公共子表达式 解决其对于Phi函数可能的破坏
Subexpression *createSubExpression(Value *lhs, Value *rhs, Opcode op){
    Subexpression *subexpression = (Subexpression*)malloc(sizeof(Subexpression));
    memset(subexpression, 0, sizeof(Subexpression));
    subexpression->lhs = lhs;
    subexpression->rhs = rhs;
    subexpression->op = op;
    return subexpression;
}

bool commonSubexpressionElimination(Function *currentFunction){
    bool effective = false;
    //runs for each BasicBlock
    BasicBlock *entry = currentFunction->entry;

    clear_visited_flag(entry);


    HashSet *workList = HashSetInit();
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        assert(block != NULL);
        block->visited = true;
        HashSetRemove(workList,block);
        effective |= commonSubexpression(block);
        if(block->true_block && block->true_block->visited == false)
            HashSetAdd(workList,block->true_block);
        if(block->false_block && block->false_block->visited == false)
            HashSetAdd(workList,block->false_block);
    }
    renameVariabels(currentFunction);
    return effective;
}

bool commonSubexpression(BasicBlock *block){
    bool effective = false;
    HashMap* commonSubExpression = HashMapInit();
    InstNode *currNode = block->head_node;

    //反正block的结尾
    while(currNode != block->tail_node){
        if(isCalculationOperator(currNode)){
            //只对加减乘除法进行替换
            Value *lhs = ins_get_lhs(currNode->inst);
            Value *rhs = ins_get_rhs(currNode->inst);
            Value *dest = ins_get_dest(currNode->inst);
            if((isImm(lhs) || isLocalVar(lhs)) && (isImm(rhs) || isLocalVar(rhs))){
                //看看现在的HashMap里面包不包含
                printf("here!\n");
                HashMapFirst(commonSubExpression);
                bool flag = false;
                Value *replace = NULL;
                for(Pair *subExpr = HashMapNext(commonSubExpression); subExpr != NULL; subExpr = HashMapNext(commonSubExpression)){
                    Subexpression *subexpression = subExpr->value;
                    switch (subexpression->op) {
                        case Add:
                            if((subexpression->lhs == lhs && subexpression->rhs == rhs) || (subexpression->rhs == lhs && subexpression->lhs == rhs)){
                                //满足条件
                                replace = subExpr->key;
                                flag = true;
                            }
                            break;
                        case Sub:
                            if(subexpression->lhs == lhs && subexpression->rhs == rhs){
                                replace = subExpr->key;
                                flag = true;
                            }
                            break;
                        case Mul:
                            if((subexpression->lhs == lhs && subexpression->rhs == rhs) || (subexpression->rhs == lhs && subexpression->lhs == rhs)){
                                //满足条件
                                replace = subExpr->key;
                                flag = true;
                            }
                            break;
                        case Div:
                            if(subexpression->lhs == lhs && subexpression->rhs == rhs){
                                replace = subExpr->key;
                                flag = true;
                            }
                            break;
                        case Mod:
                            if(subexpression->lhs == lhs && subexpression->rhs == rhs){
                                replace = subExpr->key;
                                flag = true;
                            }
                            break;
                        default:
                            assert(false);
                    }
                }

                //
                if(flag){
                    effective = true;
                    assert(replace != NULL);
                    value_replaceAll(dest,replace);
                    //TODO phi 里面的还是没有更新的

                    //删除当前InstNode;
                    InstNode *next = get_next_inst(currNode);

                    //TODO 删除之后内存并没有修改
                    delete_inst(currNode);
                    currNode = next;
                }else{
                    Subexpression *newSubExpression = createSubExpression(lhs,rhs,currNode->inst->Opcode);
                    HashMapPut( commonSubExpression,dest,newSubExpression);
                    currNode = get_next_inst(currNode);
                }
            }else{
                currNode = get_next_inst(currNode);
            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
    HashMapDeinit(commonSubExpression);
    return effective;
}