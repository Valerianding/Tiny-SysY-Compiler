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
int g = 0;

float func(int n) {
    g = g + n;
    putint(g);
    return g;
}

int main() {
    int i;
    i = getint();
    if (!func(99) && func(100)) i = 1; else i = 0;
    return 0;
}