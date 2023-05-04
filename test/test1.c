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
//int g = 1;
//int test(){
//    int a = getint();
//    int b = getint();
//    int c = 1;
//    int arr[10];
//
//    while(c < 10){
//        int temp2 = a + b;
//        int temp = temp2 + b;
//        arr[1] = temp;
//        g = temp;
//    }
//    putint(a);
//    return 0;
//}
//int sub(int a,int b){
//    int c=a-b;
//    return c;
//}
//int main(){
//    int a;
//    int b;
//    int c;
//    c=sub(a,b);
//    return 0;
//}

//int test(){
//    int a = 1;
//    int c = 2;
//    a = getint() + 1;
//    if(a > 1){
//        c = c + 1;
//        a = a + 1;
//    }else{
//        a = a - 1;
//    }
//    return c;
//}
int ints[10000];
int intt;
int chas[10000];
int chat;
int i=0, ii=1;
int c;
int get[10000];
int get2[10000];

//int isdigit(int x) {
//    if (x >= 48 && x <= 57)
//        return 1;
//    return 0;
//}
//
//int power(int b, int a) {
//    int result = 1;
//    while (a != 0) {
//        result = result * b;
//        a = a - 1;
//    }
//    return result;
//}
//
//int getstr(int get[]) {
//    int x = getch();
//    int length = 0;
//    while (x != 13 && x != 10) {
//        get[length] = x;
//        length = length + 1;
//        x = getch();
//    }
//    return length;
//}
//
//
//void intpush(int x)
//{
//    intt = intt + 1;
//    ints[intt] = x;
//}
//void chapush(int x)
//{
//    chat = chat + 1;
//    chas[chat] = x;
//}
//int intpop()
//{
//    intt = intt - 1;
//    return ints[intt + 1];
//}
//int chapop()
//{
//    chat = chat - 1;
//    return chas[chat + 1];
//}
//
//void intadd(int x)
//{
//    ints[intt] = ints[intt] * 10;
//    ints[intt] = ints[intt] + x;
//}
//
//int find()
//{
//    c = chapop();
//    get2[ii] = 32;
//    get2[ii + 1] = c;
//    ii = ii + 2;
//    if (chat == 0) return 0;
//    return 1;
//}

int test(){
    int a = 1;
    int c = 2;
    if(a > 1){
        a = a + 1;
    }else{
        a = a - 1;
    }
    return c;
}
//
//void my_putint(int a)
//{
//    int b[16], i = 0;
//    while (a > 0) {
//        b[i] = a % 10 + ascii_0;
//        a = a / 10;
//        i = i + 1;
//    }
//    while (i > 0) {
//        i = i - 1;
//        putch(b[i]);
//    }
//}
//
//int main()
//{
//    int n = my_getint();
//    while (n > 0) {
//        int m = my_getint();
//        my_putint(m); putch(10);
//        n = n - 1;
//    }
//    return 0;
//}


//int power(int b, int a) {
//    int result = 1;
//    while (a != 0) {
//        result = result * b;
//        a = a - 1;
//    }
//    return result;
//}
//
//int getstr(int get[]) {
//    int x = getch();
//    int length = 0;
//    while (x != 13 && x != 10) {
//        get[length] = x;
//        length = length + 1;
//        x = getch();
//    }
//    return length;
//}
//
//void intpush(int x)
//{
//    intt = intt + 1;
//    ints[intt] = x;
//}
//void chapush(int x)
//{
//    chat = chat + 1;
//    chas[chat] = x;
//}
//int intpop()
//{
//    intt = intt - 1;
//    return ints[intt + 1];
//}
//int chapop()
//{
//    chat = chat - 1;
//    return chas[chat + 1];
//}
//void intadd(int x)
//{
//    ints[intt] = ints[intt] * 10;
//    ints[intt] = ints[intt] + x;
//}
//
//int find()
//{
//    c = chapop();
//    get2[ii] = 32;
//    get2[ii + 1] = c;
//    ii = ii + 2;
//    if (chat == 0) return 0;
//    return 1;
//}
//
//int main()
//{
//    intt=0;
//    chat=0;
//    int lengets = getstr(get);
//    while (i < lengets)
//    {
//        if (isdigit(get[i]) == 1)
//        {
//            get2[ii] = get[i];
//            ii = ii + 1;
//        }
//        else
//        {
//            if(get[i] == 40) chapush(40);
//            if(get[i] == 94) chapush(94);
//            if(get[i] == 41)
//            {
//                c = chapop();
//                while (c != 40)
//                {
//                    get2[ii] = 32;
//                    get2[ii + 1]=c;
//                    ii = ii + 2;
//                    c = chapop();
//                }
//            }
//            if (get[i] == 43)
//            {
//                while (chas[chat] == 43 || chas[chat] == 45 || chas[chat] == 42 || chas[chat] == 47 || chas[chat] == 37 || chas[chat] == 94)
//                {
//                    if (find()==0)break;
//                }
//                chapush(43);
//            }
//            if (get[i] == 45)
//            {
//                while (chas[chat] == 43 || chas[chat] == 45 ||chas[chat] == 42 || chas[chat] == 47 || chas[chat] == 37 || chas[chat] == 94)
//                {
//                    if(find()==0)break;
//                }
//                chapush(45);
//            }
//            if(get[i] == 42)
//            {
//                while (chas[chat] == 42 || chas[chat] == 47 ||chas[chat] == 37 || chas[chat] == 94)
//                {
//                    if (find()==0)break;
//                }
//                chapush(42);
//            }
//            if (get[i] == 47)
//            {
//                while (chas[chat] == 42 || chas[chat] == 47 || chas[chat] == 37 || chas[chat] == 94)
//                {
//                    if (find()==0)break;
//                }
//                chapush(47);
//            }
//            if (get[i] == 37)
//            {
//                while (chas[chat] == 42 || chas[chat] == 47 || chas[chat] == 37 || chas[chat] == 94)
//                {
//                    if (find()==0)break;
//                }
//                chapush(37);
//            }
//            get2[ii] = 32;
//            ii = ii + 1;
//        }
//        i = i + 1;
//    }
//    while(chat > 0)
//    {
//        int c = chapop();
//        get2[ii] = 32;
//        get2[ii + 1]=c;
//        ii = ii + 2;
//    }
//    get2[ii]= 64;
//    i = 1;
//    while (get2[i] != 64)
//    {
//        if (get2[i] == 43 || get2[i] == 45 || get2[i] == 42 || get2[i] == 47 || get2[i] == 37 || get2[i] == 94)
//        {
//            int a=intpop();int b=intpop();int c;
//            if (get2[i] == 43) c = a + b;
//            if (get2[i] == 45) c = b - a;
//            if (get2[i] == 42) c = a * b;
//            if (get2[i] == 47) c = b / a;
//            if (get2[i] == 37) c = b % a;
//            if (get2[i] == 94) c = power(b,a);
//            intpush(c);
//        }
//        else
//        {
//            if(get2[i] != 32)
//            {
//                intpush(get2[i] - 48);
//                ii=1;
//                while(get2[i+ii] != 32)
//                {
//                    intadd(get2[i+ii] - 48);
//                    ii = ii + 1;
//                }
//                i = i + ii-1;
//            }
//        }
//        i = i + 1;
//    }
//    putint(ints[1]);
//    return 0;
//}
//phi的计算
//int main(){
//    int a = 5;
//    int b = 10;
//    int c = 15;
//    while(c > 5){
//        int temp = a;
//        a = b;
//        b = temp;
//        c = c - 1;
//    }
//    return a;
//}