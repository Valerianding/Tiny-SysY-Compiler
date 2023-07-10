int test(int arr[][100]){
    return arr[1][2];
}

int main(){
    int arr[10][10][100];
    test(arr[1][10]);
    return 1;
}