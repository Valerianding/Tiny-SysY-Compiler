#include "tree_balancing.h"

//1. 定root,在语法树上做的话，其实最终队列里只有一个大root,主要是要将其他定下的root加到root_set里

bool add_roots(past* child,past* parent, HashSet* root_set, HashMap* rank_map){
    if((*child)->iVal == '-' || (*child)->iVal == '/' || (*child)->iVal == '%' || (*child)->iVal == '!')
        return false;

    if((*child)->iVal != (*parent)->iVal){
        int *rank_ = malloc(4);
        *rank_ = -1;
        HashMapPut(rank_map,*child,rank_);
        HashSetAdd(root_set,child);
    }

    bool ret1 = true, ret2 = true;
    if((*child)->left && strcmp(bstr2cstr((*child)->left->nodeType, '\0'), "expr") == 0)
        ret1 = add_roots(&(*child)->left,child,root_set,rank_map);
    if((*child)->left && strcmp(bstr2cstr((*child)->right->nodeType, '\0'), "expr") == 0)
        ret2 = add_roots(&(*child)->right,child,root_set,rank_map);
    return ret1 && ret2;
}

//bool为1代表没有遍历到-或/, +与*具有结合律与交换律，可以balance。不能balance则返回0
bool find_roots(past* expr, Queue* queue, HashMap* rank_map,HashSet* root_set)
{
    if((*expr)->iVal == '-' || (*expr)->iVal == '/' || (*expr)->iVal == '%' || (*expr)->iVal == '!')
        return false;

    //先让expr进入root队列
    QueuePush(queue,expr);
    int *rank = malloc(4);
    *rank = -1;
    HashMapPut(rank_map,*expr,rank);
    HashSetAdd(root_set,expr);

    bool ret1 = true, ret2 = true;
    if((*expr)->left && strcmp(bstr2cstr((*expr)->left->nodeType, '\0'), "expr") == 0)
        ret1 = add_roots(&(*expr)->left,expr,root_set,rank_map);
    if((*expr)->right && strcmp(bstr2cstr((*expr)->right->nodeType, '\0'), "expr") == 0)
        ret2 = add_roots(&(*expr)->right,expr,root_set,rank_map);

    return ret1 && ret2;
}

void Rebuild(Queue* q, past* expr,HashMap* rank_map, int Op){
    while(QueueSize(q) != 0){
        past nl , nr;
        QueueFront(q, (void **) &nl);
        QueuePop(q);
        QueueFront(q, (void **) &nr);
        QueuePop(q);

        //if nl and nr are both constant
        if((strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0) && (strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0)){
            //fold
            past fold = NULL;
            if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0 && Op == '+')       //整数加法
                fold = newNumInt(nl->iVal + nr->iVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0)            //整数乘法
                fold = newNumInt(nl->iVal * nr->iVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0 && Op == '+')      //小数加法
                fold = newNumFloat(nl->fVal + nr->fVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0)        //乘法
                fold = newNumFloat(nl->fVal * nr->fVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0 && Op == '+')      //小数加法
                fold = newNumFloat(nl->fVal + (float)nr->iVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_float") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_int") == 0)        //乘法
                fold = newNumFloat(nl->fVal * (float)nr->iVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0 && Op == '+')      //小数加法
                fold = newNumFloat((float)nl->iVal + nr->fVal);
            else if(strcmp(bstr2cstr(nl->nodeType, '\0'), "num_int") == 0 && strcmp(bstr2cstr(nr->nodeType, '\0'), "num_float") == 0)        //乘法
                fold = newNumFloat((float)nl->iVal * nr->fVal);

            //即最终可化简成一个常数
            if(QueueSize(q) == 0){
                //emit "root" <--- fold result 将整个root(expr)用最后算出的常值代替
                int *rank = malloc(4);
                *rank = 0;
                HashMapPut(rank_map,fold,rank);
                *expr = fold;
            } else {
                int *rank = malloc(4);
                *rank = 0;
                HashMapPut(rank_map, fold,rank);
                QueuePush(q, fold);
            }
        } else {
            past NT = newExpr(nl,Op,nr);

            if(QueueSize(q) == 0){
                // NT <--- root
                *expr = NT;
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

int flatten(past* var, Queue* q,HashMap* rank_map,HashSet* root_set){
    //if var is a constant, rank = 0
    if(strcmp(bstr2cstr((*var)->nodeType, '\0'), "num_int") == 0 || strcmp(bstr2cstr((*var)->nodeType, '\0'), "num_float") == 0){
        int *rank = malloc(4);
        *rank = 0;
        HashMapPut(rank_map,*var,rank);
        QueuePush(q,*var);
    } else if(strcmp(bstr2cstr((*var)->nodeType, '\0'), "expr") != 0){
        int *rank = malloc(4);
        *rank = 1;
        HashMapPut(rank_map,*var,rank);
        QueuePush(q,*var);
    }
    else if(strcmp(bstr2cstr((*var)->nodeType, '\0'), "expr") == 0 && HashSetFind(root_set, var)){
        //root
        balance(var,rank_map,root_set);
        QueuePush(q,*var);
    } else {
        return flatten(&(*var)->left, q,rank_map,root_set) + flatten(&(*var)->right, q, rank_map,root_set);
    }

    int *rank = HashMapGet(rank_map,*var);
    return *rank;
}

void balance(past *root, HashMap* rank_map,HashSet* root_set){
    int *rank = HashMapGet(rank_map,*root);
    if(*rank >= 0)
        return;

    //1. first flatten the tree
    //new queue of names
    Queue *q = QueueInit();
    *rank = flatten(&(*root)->left,q, rank_map,root_set) + flatten(&(*root)->right,q,rank_map,root_set);

    return Rebuild(q,root,rank_map,(*root)->iVal);
}

void tree_balancing(past* expr)
{
    //这个queue用来保存expr中定的root
    Queue *queue_root  = QueueInit();

    //这个set用来保存root的集合，方便查看是不是root节点
    HashSet* root_set = HashSetInit();

    //用于保存每个root的rank值,初始都是-1
    HashMap* rank_map = HashMapInit();

    if(find_roots(expr,queue_root,rank_map,root_set)){
        while(QueueSize(queue_root)!=0){
            past* root ;
            QueueFront(queue_root,(void**)&root);
            QueuePop(queue_root);
            balance(root,rank_map,root_set);
        }
    }

   // showAst(*expr,0);
}