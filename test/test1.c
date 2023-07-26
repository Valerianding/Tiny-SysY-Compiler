int j = 0;
int test(){
    int b[16];
    int i = getint();
    j = getint();
    while(i > j){
        b[i] = i % 10 + j;
        j = 1;
        i = i + 1;
    }
}