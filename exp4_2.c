#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_VERTEX 35
#define INF INT_MAX
typedef struct {
    int arcs[MAX_VERTEX][MAX_VERTEX]; 
    int vexNum;                        
    int arcNum;                       
} MGraph;

typedef struct {
    int u, v;      // 边的两个顶点
    int weight;    // 边的权值
} Edge;

// Prim算法 
void Prim(MGraph *G, int start) {
    int lowcost[MAX_VERTEX];   // 记录到各顶点的最小代价
    int closest[MAX_VERTEX];   // 记录最小代价边的邻接顶点
    int visited[MAX_VERTEX];   // 记录是否已加入生成树
    int totalCost = 0;    
    // 初始化
    for (int i = 1; i <= G->vexNum; i++) {
        lowcost[i] = G->arcs[start][i];
        closest[i] = start;
        visited[i] = 0;
    }
    visited[start] = 1;
    
    printf("Prim算法最小生成树的边:\n");    
    // 选择n-1条边
    for (int i = 1; i < G->vexNum; i++) {
        int minCost = INF;
        int k = -1;
        
        // 找最小代价的顶点
        for (int j = 1; j <= G->vexNum; j++) {
            if (!visited[j] && lowcost[j] < minCost) {
                minCost = lowcost[j];
                k = j;
            }
        }
        
        if (k == -1) {
            printf("图不连通，无法生成最小生成树\n");
            return;
        }        
        // 输出选中的边
        printf("(%d, %d) 权值: %d\n", closest[k], k, minCost);
        totalCost += minCost;
        visited[k] = 1;        
        // 更新lowcost和closest
        for (int j = 1; j <= G->vexNum; j++) {
            if (!visited[j] && G->arcs[k][j] < lowcost[j]) {
                lowcost[j] = G->arcs[k][j];
                closest[j] = k;
            }
        }
    }    
    printf("最小生成树总代价: %d\n", totalCost);
}

// 并查集（用于Kruskal）
int parent[MAX_VERTEX];

void InitUF(int n) {
    for (int i = 1; i <= n; i++) {
        parent[i] = i;
    }
}
int Find(int x) {
    if (parent[x] != x) {
        parent[x] = Find(parent[x]);  // 路径压缩
    }
    return parent[x];
}
void Union(int x, int y) {
    int px = Find(x);
    int py = Find(y);
    if (px != py) {
        parent[px] = py;
    }
}

// 边排序（按权值升序）
int cmp(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

// Kruskal算法 
void Kruskal(MGraph *G) {
    Edge edges[MAX_VERTEX * MAX_VERTEX];
    int edgeCount = 0;
    
    // 收集所有边（无向图只取上三角）
    for (int i = 1; i <= G->vexNum; i++) {
        for (int j = i + 1; j <= G->vexNum; j++) {
            if (G->arcs[i][j] != INF && G->arcs[i][j] != 0) {
                edges[edgeCount].u = i;
                edges[edgeCount].v = j;
                edges[edgeCount].weight = G->arcs[i][j];
                edgeCount++;
            }
        }
    }    
    // 按权值排序
    qsort(edges, edgeCount, sizeof(Edge), cmp);
    
    // 初始化并查集
    InitUF(G->vexNum);
    
    printf("Kruskal算法最小生成树的边:\n");    
    int totalCost = 0;
    int selectedCount = 0;
    
    // 选择n-1条边
    for (int i = 0; i < edgeCount && selectedCount < G->vexNum - 1; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int w = edges[i].weight;        
        // 检查是否形成环
        if (Find(u) != Find(v)) {
            Union(u, v);
            printf("(%d, %d) 权值: %d\n", u, v, w);
            totalCost += w;
            selectedCount++;
        }
    }
    
    if (selectedCount < G->vexNum - 1) {
        printf("图不连通，无法生成最小生成树\n");
        return;
    }
    
    printf("最小生成树总代价: %d\n", totalCost);
}

int main() {
    MGraph G;
    int n, m;
    int u, v, w;    
    // 输入顶点数和边数
    printf("请输入顶点数和边数: ");
    scanf("%d %d", &n, &m);
    G.vexNum = n;
    G.arcNum = m;    
    // 初始化邻接矩阵
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (i == j) {
                G.arcs[i][j] = 0;
            } else {
                G.arcs[i][j] = INF;
            }
        }
    }    
    // 输入边（带权值）
    printf("请输入%d条边（格式: 顶点1 顶点2 权值）:\n", m);
    for (int k = 0; k < m; k++) {
        scanf("%d %d %d", &u, &v, &w);
        G.arcs[u][v] = w;
        G.arcs[v][u] = w;  // 无向图
    }
    
    printf("\n===== 通讯网的最小代价生成树 =====\n\n");
    
    // Prim算法
    printf("--- Prim算法---\n");
    Prim(&G, 1);
    
    printf("\n");
    
    // Kruskal算法
    printf("--- Kruskal算法 ---\n");
    Kruskal(&G);    
    return 0;
}
