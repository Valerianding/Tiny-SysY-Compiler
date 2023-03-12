
//
// Created by Valerian on 2023/3/10.
//

int main(){
    int a[10] = {1,2,3,4,5};
    if(a[0] > 1){
        a[0] = a[0] + 1;
    }else{
        a[0] = a[0] - 1;
    }
    return a[0];
}

