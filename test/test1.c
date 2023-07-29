int main(){
    int arr[11][11];

    int a = 0;
    while(a < 10){
        int b = 0;
        while(b < 10){
            arr[10][b] = 3;
            b = b + 1;
        }
        a = a + 1;
    }
    return arr[10][2];
}