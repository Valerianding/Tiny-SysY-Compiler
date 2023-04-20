//////const int len = 20;
//
//int main()
//{
//    int i, j, t, n, temp;
//    int mult1[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
//    int mult2[20] = {2, 3, 4, 2, 5, 7 ,9 ,9, 0, 1, 9, 8, 7, 6, 4, 3, 2, 1, 2, 2};
//
//    int len1 = 20;
//    int len2 = 20;
//    int c1[20 + 5];
//    int c2[20 + 5];
//    int result[20 * 2] = {};
//    i = 0;
//    while (i < len1) {
//        c1[i] = mult1[i];
//        i = i + 1;
//    }
//
//    i = 0;
//    while (i < len2) {
//        c2[i] = mult2[i];
//        i = i + 1;
//    }
//
//    n = len1 + len2 - 1;
//
//    i = 0;
//    while (i <= n) {
//        result[i]=0;
//        i = i + 1;
//    }
//
//    temp=0;
//
//    i = len2 - 1;
//    while (i > -1) {
//        t = c2[i];
//        j = len1 - 1;
//        while (j > -1) {
//            temp = result[n] + t * c1[j];
//            if(temp >= 10) {
//                result[n] = (temp);
//                result[n-1] = result[n-1] + temp / 10;
//            }
//            else
//                result[n] = temp;
//            j = j - 1;
//            n = n - 1;
//        }
//        n = n + len1 - 1;
//        i = i - 1;
//    }
//
//    if(result[0] != 0)
//        putint(result[0]);
//
//    i = 1;
//    while (i <= len1 + len2 - 1) {
//        putint(result[i]);
//        i = i + 1;
//    }
//
//    return 0;
//}
//
////int main(){
////    int a = getint();
////    int b = getint();
////    int c = 1;
////    int temp;
////    while(c <= 1){
////        temp = a + b;
////        temp = a + b;
////        c = c + 1;
////    }
////}

int a, b, d;

int set_a(int val) { a = val; return a; }
int set_b(int val) { b = val; return b; }
int set_d(int val) { d = val; return d; }

int main()
{
    a = 2; b = 3;
    if (set_a(0) && set_b(1)) {}
    putint(a); putch(32);
    putint(b); putch(32);

    a = 2; b = 3;
    if (set_a(0) && set_b(1)) ;
    putint(a); putch(32);
    putint(b); putch(10);

    const int c = 1;
    d = 2;
    if (c >= 1 && set_d(3)) ;
    putint(d); putch(32);
    if (c <= 1 || set_d(4)) {}
    putint(d); putch(10);

    if (16 >= (3 - (2 + 1))) { putch(65); }
    if ((25 - 7) != (36 - 6 * 3)) putch(66);
    if (1 < 8 != 7 % 2) { putch(67); }
    if (3 > 4 == 0) { putch(68); }
    if (1 == 0x66 <= 077) putch(69);
    if (5 - 6 == -!0) putch(70);
    putch(10);

    int i0 = 0, i1 = 1, i2 = 2, i3 = 3, i4 = 4;
    while (i0 && i1) putch(32);
    if (i0 || i1) putch(67);
    if (i0 >= i1 || i1 <= i0) putch(72);
    if (i2 >= i1 && i4 != i3) { putch(73); }
    if (i0 == !i1 && i3 < i3 || i4 >= i4) { putch(74); }
    if (i0 == !i1 || i3 < i3 && i4 >= i4) putch(75);
    putch(10);

    return 0;
}
