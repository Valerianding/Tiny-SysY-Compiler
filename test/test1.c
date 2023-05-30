int a[5];
int b = 10;
int main() {
    a[4] = 1;
    a[4] = 2;
    a[4] = a[4] + 1;
    b = b + 1;
    int ans = a[2 * 2] + a[2 * 2] + a[2 * 2] + b;
    return ans;
}