#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTEX 35
#define INF INT_MAX

// ==================== 邻接矩阵存储结构 ====================
typedef struct {
    int arcs[MAX_VERTEX][MAX_VERTEX]; // 邻接矩阵（存储权值/距离）
    int vexNum;                        // 顶点数（站点数）
    int arcNum;                        // 边数（线路数）
} MGraph;

// ==================== Dijkstra算法 ====================
void Dijkstra(MGraph *G, int start, int end) {
    int dist[MAX_VERTEX];      // 从起点到各顶点的最短距离
    int visited[MAX_VERTEX];   // 是否已确定最短路径
    int path[MAX_VERTEX];      // 记录路径（前驱顶点）
    
    // 初始化
    for (int i = 1; i <= G->vexNum; i++) {
        dist[i] = G->arcs[start][i];
        visited[i] = 0;
        if (dist[i] < INF && dist[i] != 0) {
            path[i] = start;
        } else {
            path[i] = -1;
        }
    }
    
    dist[start] = 0;
    visited[start] = 1;
    path[start] = -1;
    
    // 迭代n-1次，每次确定一个顶点的最短路径
    for (int i = 1; i < G->vexNum; i++) {
        int minDist = INF;
        int u = -1;
        
        // 找到未访问顶点中距离最小的
        for (int j = 1; j <= G->vexNum; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        
        if (u == -1) break;  // 剩余顶点不可达
        
        visited[u] = 1;
        
        // 更新经过u到达其他顶点的距离
        for (int v = 1; v <= G->vexNum; v++) {
            if (!visited[v] && G->arcs[u][v] < INF) {
                if (dist[u] + G->arcs[u][v] < dist[v]) {
                    dist[v] = dist[u] + G->arcs[u][v];
                    path[v] = u;
                }
            }
        }
    }
    
    // 输出结果
    printf("\n===== 最短路径结果 =====\n");
    
    if (dist[end] == INF) {
        printf("从站点 %d 到站点 %d 没有可达路径\n", start, end);
        return;
    }
    
    printf("从站点 %d 到站点 %d 的最短距离: %d\n", start, end, dist[end]);
    
    // 输出路径（逆向追溯）
    int route[MAX_VERTEX];
    int routeLen = 0;
    int curr = end;
    
    while (curr != -1) {
        route[routeLen++] = curr;
        curr = path[curr];
    }
    
    printf("最短路径: ");
    for (int i = routeLen - 1; i >= 0; i--) {
        printf("%d", route[i]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

// ==================== 主函数 ====================
int main() {
    MGraph G;
    int n, m;
    int u, v, w;
    int start, end;
    
    // 输入站点数和线路数
    printf("请输入站点数和线路数: ");
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
    
    // 输入线路（带距离/权值）
    printf("请输入%d条线路（格式: 站点1 站点2 距离）:\n", m);
    for (int k = 0; k < m; k++) {
        scanf("%d %d %d", &u, &v, &w);
        G.arcs[u][v] = w;
        G.arcs[v][u] = w;  // 无向图（双向线路）
    }
    
    // 输入始发站和终点站
    printf("请输入始发站和终点站: ");
    scanf("%d %d", &start, &end);
    
    // Dijkstra算法求最短路径
    Dijkstra(&G, start, end);
    
    return 0;
}
