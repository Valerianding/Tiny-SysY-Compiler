void mem_move(int src[], int dst[], int n){
    int i = 0;
    while (i < n){
        dst[i] = src[i];
        i = i + 1;
    }
}