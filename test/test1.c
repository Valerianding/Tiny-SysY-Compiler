//int sum(int n,int m){
//    int sum = 0, i = 0, j = 0;
//    while(1){
//        sum = sum + i * j;
//        if( j < m ){
//            j = j + 1;
//            continue;
//        }
//        if(i < n){
//            i = i + 1;
//            j = 0;
//            continue;
//        }else{
//            break;
//        }
//    }
//    return sum;
//}


//example of wrong
//int main(){
//    int i = 0;
//    int a = getint();
//    int b = getint();
//    int m = a + b;
//    int j = 1;
//    while(i > 10){
//        if(a > 1){
//            break;
//        }
//        if(m == a){
//            break;
//        }
//        while(j < 10){
//            if(j == 6){
//                continue;
//            }else{
//                break;
//            }
//            j = j + 1;
//        }
//        i = i + 1;
//    }
//    return j;
//}


//test float
//int g = 0;
//
//float func(int n) {
//    g = g + n;
//    putint(g);
//    return g;
//}
//
//int main() {
//    int i;
//    i = getint();
//    if (!func(99) && func(100)) i = 1; else i = 0;
//    return 0;
//}

const int maxN = 1005;
int parent[maxN];

int find(int root) {
    if (parent[root] == root)
        return root;
    else {
        parent[root] = find(parent[root]);
        return parent[root];
    }
}

void merge(int p, int q) {
    int root_p, root_q;
    root_p = find(p);
    root_q = find(q);
    if (root_p != root_q) {
        parent[root_q] = root_p;
    }
    return;
}

int main() {
    int n = getint(), m = getint(), i;
    int p, q;
    i = 0;
    while (i < n) {
        parent[i] = i;
        i = i + 1;
    }
    i = 0;
    while (i < m) {
        p = getint();
        q = getint();
        merge(p, q);
        i = i + 1;
    }

    int clusters = 0;
    i = 0;
    while (i < n) {
        if (parent[i] == i) clusters = clusters + 1;
        i = i + 1;
    }
    putint(clusters);
    return 0;
}