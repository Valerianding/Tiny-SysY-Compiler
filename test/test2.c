const float g = 3.14;
float g1;
int i1;
int global[1];
int test(float x){
    g1 = 3.14;
    int xx=3.14;
    int xx1 = g;
    int xx2 = g1;
    i1 = g1;
    int ans = 3.5 * 2 + 3.5 * 2 + xx * xx1 * xx2;
    float arr[1];
    arr[1] = 3.14;
    int which = arr[1];
    global[0] = arr[1];
    return ans * which;
}
