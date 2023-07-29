int test(){
    int arr[10];

    int a = getint();
    while(a < 10){
        int b = 0;
        while(b < 10){
            arr[b] = 3;
            b = b + 1;
        }
        a = a + 1;
    }
}