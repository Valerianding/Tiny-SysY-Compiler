/*
 * Max flow EK with DFS.
 */
const int INF = 0x70000000;

int size[10];
int to[10][10];
int cap[10][10];
int rev[10][10];
int used[10];

int max_flow(int s, int t)
{
    int flow = 0;

    while (1) {
        my_memset(used, 0, 10);

        int f = dfs(s, t, INF);
        if (f == 0)
            return flow;
        flow = flow + f;
    }
}