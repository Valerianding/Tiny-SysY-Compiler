/* Type your code here, or load an example. */
int square(int num,float a, int b,int c,int arr[]) {
    int num1 = 1.3;
    float num2 = a + num1;
    arr[0] = num2;
    return num * num;
}
int main(){
    int result[100];
    float c = square(1,3.1,1,1,result);
    int d = c + result[0];
    return result[1];
}

 