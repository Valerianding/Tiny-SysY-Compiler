#include "tree_balancing.h"

//1. 定root(从后往前进root),到时候一个一个出来balance吧


//bool为1代表没有遍历到-或/, +与*具有结合律与交换律，可以balance。不能balance则返回0
bool find_roots(past expr, Queue* queue, HashMap* rank_map,HashSet* root_set)
{
    //TODO
    //假设就只有expr一个root
    QueuePush(queue,expr);
    int *rank = malloc(4);
    *rank = -1;
    HashMapPut(rank_map,expr,rank);
    HashSetAdd(root_set,expr);
    return true;
}

past Rebuild(Queue* q, past expr,HashMap* rank_map, int Op){
    while(QueueSize(q) != 0){
        past nl , nr;
        QueueFront(q, (void **) &nl);
        QueuePop(q);
        QueueFront(q, (void **) &nr);
        QueuePop(q);

        //if nl and nr are both constant
        if((strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0) && (strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0)){
            //fold
            past fold = newNumInt(nl->iVal + nr->iVal);

            //即最终可化简成一个常数
            if(QueueSize(q) == 0){
                //emit "root" <--- fold result 将整个root(expr)用最后算出的常值代替
                int *rank = malloc(4);
                *rank = 0;
                HashMapPut(rank_map,fold,rank);
                return fold;
            } else {
                int *rank = malloc(4);
                *rank = 0;
                HashMapPut(rank_map, fold,0);
                QueuePush(q, fold);
            }
        } else {
            past NT = newExpr(nl,Op,nr);

            if(QueueSize(q) == 0){
                // NT <--- root
                return NT;
            }
            //rank(NT) <--rank(nl) + rank(nr)
            int *rank = malloc(4);
            int *rank_nl = HashMapGet(rank_map,nl);
            int *rank_nr = HashMapGet(rank_map,nr);
            *rank = *rank_nl + *rank_nr;
            HashMapPut(rank_map, NT,rank);

            if(QueueSize(q) != 0){
                QueuePush(q,NT);
            }
        }
    }
}

int flatten(past var, Queue* q,HashMap* rank_map,HashSet* root_set){
    //if var is a constant, rank = 0
    if(strcmp(bstr2cstr(var->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(var->nodeType, '\0'), "num_float") == 0){
        int *rank = malloc(4);
        *rank = 0;
        HashMapPut(rank_map,var,rank);
        QueuePush(q,var);
    } else if(strcmp(bstr2cstr(var->nodeType, '\0'), "expr") != 0){
        int *rank = malloc(4);
        *rank = 1;
        HashMapPut(rank_map,var,rank);
        QueuePush(q,var);
    }
    else if(strcmp(bstr2cstr(var->nodeType, '\0'), "expr") == 0 && HashSetFind(root_set, var)){
        //root
        balance(var,rank_map,root_set);
        QueuePush(q,var);
    } else {
        return flatten(var->left, q,rank_map,root_set) + flatten(var->right, q, rank_map,root_set);
    }

    int *rank = HashMapGet(rank_map,var);
    return *rank;
}

past balance(past root, HashMap* rank_map,HashSet* root_set){
    int *rank = HashMapGet(rank_map,root);
    if(*rank >= 0)
        return root;

    //1. first flatten the tree
    //new queue of names
    Queue *q = QueueInit();
    *rank = flatten(root->left,q, rank_map,root_set) + flatten(root->right,q,rank_map,root_set);

    return Rebuild(q,root,rank_map,root->iVal);
}

past tree_balancing(past expr)
{
    //这个queue用来保存expr中定的root
    Queue *queue_root  = QueueInit();

    //这个set用来保存root的集合，方便查看是不是root节点
    HashSet* root_set = HashSetInit();

    //用于保存每个root的rank值,初始都是-1
    HashMap* rank_map = HashMapInit();

    past ret = NULL;
    if(find_roots(expr,queue_root,rank_map,root_set)){
        while(QueueSize(queue_root)!=0){
            past root ;
            QueueFront(queue_root,(void**)&root);
            QueuePop(queue_root);
            ret = balance(root,rank_map,root_set);
        }
    }

    if(ret!=NULL)
        showAst(ret,0);
    return ret;
}