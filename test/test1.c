int size[10];
int to[10][10];
int cap[10][10];
int rev[10][10];
int used[10];

int dfs(int s, int t, int f)
{
    if (s == t)
        return f;
    used[s] = 1;

    int i = 0;
    while (i < size[s]) {
        if (used[to[s][i]]) { i = i + 1; continue; }
        if (cap[s][i] <= 0) { i = i + 1; continue; }

        int min_f;
        if (f < cap[s][i])
            min_f = f;
        else
            min_f = cap[s][i];
        int d = dfs(to[s][i], t, min_f);

        if (d > 0) {
            cap[s][i] = cap[s][i] - d;
            cap[to[s][i]][rev[s][i]] = cap[to[s][i]][rev[s][i]] + d;
            return d;
        }
        i = i + 1;
    }
    return 0;
}
