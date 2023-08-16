int main(){
    int a = getint();
    int b = getint();
    int arr[10] = {};
    int arr1[10] = {};
    while(a < b){
        if(a == 4){
            break;
        }
        arr[a * 3 + 4] = arr1[3 * a + 1];
        a = a + 1;
    }
    return arr[1];
}