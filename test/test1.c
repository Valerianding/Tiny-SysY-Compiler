


int memmove(int dst[], int dst_pos, int src[], int len){
    int i = 0;
    while (i < len){
        dst[i] = src[i];
        i = i + 1;
    }
    return i;
}