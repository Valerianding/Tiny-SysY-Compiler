int a[10] = {1};
int b[10] = {1};
int c = 10;
void test2(){
    b[1] = b[1] + 1;
    c = c + 10;
}

int test(){
    a[1] = a[1] + 1;
    test2();
}

int main(){
    int arr[10][10];
    test(arr[0]);
}