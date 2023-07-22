int main(){
    // int n = getint();
    int n = 1000;
    int x = 0;
    int temp = getint();
    int arr[10];
    while (n > 0){
        int index = x * 2 + 3 ;
        arr[index] = 1;
        x = x + temp;
        n = n + 1;
    }
    return x;
}