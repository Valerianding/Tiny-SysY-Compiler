int size[10];
int to[10][10];
int cap[10][10];
int rev[10][10];
int used[10];

int test(int t[]){
    t[1] = t[1] + 1;
    return t[1];
}

int dfs(int s[]){
    int a = s[1];
    int res;
    if(a > 1){
        res = test(s);
    }else{
        res = test(s) + 1;
    }
    return 0;
}
