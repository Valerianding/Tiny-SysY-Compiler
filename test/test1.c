int test(){
    int arr[5][2000];
    arr[1][1999] = 1;
    return arr[1][1999] + arr[1][1999] + arr[1][1999] + arr[1][1999] + arr[1][1999] + arr[1][1999] + arr[1][1999] + arr[1][1999];
}