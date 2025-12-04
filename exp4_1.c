#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTEX 35

// ==================== 邻接矩阵存储结构 ====================
typedef struct {
    int arcs[MAX_VERTEX][MAX_VERTEX]; // 邻接矩阵
    int vexNum;                        // 顶点数
    int arcNum;                        // 边数
} MGraph;

// ==================== 队列结构（用于BFS）====================
typedef struct {
    int data[MAX_VERTEX];
    int front, rear;
} Queue;

void InitQueue(Queue *Q) {
    Q->front = Q->rear = 0;
}

int IsEmpty(Queue *Q) {
    return Q->front == Q->rear;
}

void EnQueue(Queue *Q, int x) {
    Q->data[Q->rear++] = x;
}

int DeQueue(Queue *Q) {
    return Q->data[Q->front++];
}

// ==================== 全局变量 ====================
int visited[MAX_VERTEX];  // 访问标记数组
int dfsResult[MAX_VERTEX], dfsCount;  // DFS结果
int bfsResult[MAX_VERTEX], bfsCount;  // BFS结果

// ==================== 深度优先搜索（递归）====================
void DFS(MGraph *G, int v) {
    // 访问当前顶点
    dfsResult[dfsCount++] = v;
    visited[v] = 1;
    
    // 按编号从小到大顺序访问邻接顶点
    for (int j = 1; j <= G->vexNum; j++) {
        if (G->arcs[v][j] == 1 && !visited[j]) {
            DFS(G, j);
        }
    }
}

void DFSTraverse(MGraph *G, int start) {
    // 初始化访问标记
    for (int i = 1; i <= G->vexNum; i++) {
        visited[i] = 0;
    }
    dfsCount = 0;
    
    // 从起始顶点开始DFS
    DFS(G, start);
}

// ==================== 广度优先搜索 ====================
void BFS(MGraph *G, int start) {
    Queue Q;
    InitQueue(&Q);
    
    // 访问起始顶点
    bfsResult[bfsCount++] = start;
    visited[start] = 1;
    EnQueue(&Q, start);
    
    while (!IsEmpty(&Q)) {
        int v = DeQueue(&Q);
        
        // 按编号从小到大顺序访问邻接顶点
        for (int j = 1; j <= G->vexNum; j++) {
            if (G->arcs[v][j] == 1 && !visited[j]) {
                bfsResult[bfsCount++] = j;
                visited[j] = 1;
                EnQueue(&Q, j);
            }
        }
    }
}

void BFSTraverse(MGraph *G, int start) {
    // 初始化访问标记
    for (int i = 1; i <= G->vexNum; i++) {
        visited[i] = 0;
    }
    bfsCount = 0;
    
    // 从起始顶点开始BFS
    BFS(G, start);
}

// ==================== 主函数 ====================
int main() {
    MGraph G;
    int n, m, s;
    int a, b;
    
    // 输入顶点数和边数
    printf("请输入顶点数和边数: \n");
    scanf("%d %d", &n, &m);
    G.vexNum = n;
    G.arcNum = m;
    
    // 初始化邻接矩阵
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            G.arcs[i][j] = 0;
        }
    }
    
    // 输入边
    printf("请输入边的两个顶点 (格式: 顶点1 顶点2): \n");
    for (int k = 0; k < m; k++) {
        
        scanf("%d %d", &a, &b);
        G.arcs[a][b] = 1;
        G.arcs[b][a] = 1;  // 无向图
    }
    
    // 输入起始顶点
    printf("请输入起始顶点: ");
    scanf("%d", &s);
    
    // DFS遍历
    DFSTraverse(&G, s);
    
    // BFS遍历
    BFSTraverse(&G, s);
    
    // 输出DFS结果
    printf("DFS遍历结果: ");
    for (int i = 0; i < dfsCount; i++) {
        if (i > 0) printf(" ");
        printf("%d", dfsResult[i]);
    }
    printf("\n");
    
    // 输出BFS结果
    printf("BFS遍历结果: ");
    for (int i = 0; i < bfsCount; i++) {
        if (i > 0) printf(" ");
        printf("%d", bfsResult[i]);
    }
    printf("\n");
    
    return 0;
}
