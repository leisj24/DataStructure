#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 500
#define EMPTY -1
#define DELETED -2

// ==================== 线性探测法哈希表 HT1 ====================
typedef struct {
    int *data;
    int size;
    int count;
    int p;
} HashTable1;

// ==================== 拉链法哈希表 HT2 ====================
typedef struct Node {
    int key;
    struct Node *next;
} Node;

typedef struct {
    Node **data;
    int size;
    int count;
    int p;
} HashTable2;

// ==================== 辅助函数 ====================

int isPrime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

int getPrime(int n) {
    while (n > 1 && !isPrime(n)) {
        n--;
    }
    return n;
}

int hash(int key, int p) {
    return key % p;
}

// ==================== 线性探测法 HT1 操作 ====================

HashTable1* initHashTable1(int size) {
    HashTable1 *ht = (HashTable1*)malloc(sizeof(HashTable1));
    ht->size = size;
    ht->count = 0;
    ht->p = getPrime(size);
    ht->data = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        ht->data[i] = EMPTY;
    }
    return ht;
}

// 插入并返回探测次数
int insertHT1(HashTable1 *ht, int key, int *steps) {
    if (ht->count >= ht->size) {
        *steps = 0;
        return -1;
    }
    
    int addr = hash(key, ht->p);
    int i = 0;
    
    while (ht->data[(addr + i) % ht->size] != EMPTY && 
           ht->data[(addr + i) % ht->size] != DELETED) {
        if (ht->data[(addr + i) % ht->size] == key) {
            *steps = i + 1;
            return (addr + i) % ht->size;
        }
        i++;
        if (i >= ht->size) {
            *steps = 0;
            return -1;
        }
    }
    
    int pos = (addr + i) % ht->size;
    ht->data[pos] = key;
    ht->count++;
    *steps = i + 1;
    return pos;
}

int searchHT1(HashTable1 *ht, int key, int *comparisons) {
    int addr = hash(key, ht->p);
    int i = 0;
    *comparisons = 0;
    
    while (i < ht->size) {
        int pos = (addr + i) % ht->size;
        (*comparisons)++;
        
        if (ht->data[pos] == EMPTY) {
            return -1;
        }
        if (ht->data[pos] == key) {
            return pos;
        }
        i++;
    }
    return -1;
}

double calcASL_Success_HT1(HashTable1 *ht) {
    if (ht->count == 0) return 0;
    
    int totalComparisons = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->data[i] != EMPTY && ht->data[i] != DELETED) {
            int comparisons;
            searchHT1(ht, ht->data[i], &comparisons);
            totalComparisons += comparisons;
        }
    }
    return (double)totalComparisons / ht->count;
}

double calcASL_Fail_HT1(HashTable1 *ht) {
    int totalComparisons = 0;
    
    for (int i = 0; i < ht->p; i++) {
        int j = 0;
        while (ht->data[(i + j) % ht->size] != EMPTY) {
            j++;
            if (j >= ht->size) break;
        }
        totalComparisons += (j + 1);
    }
    return (double)totalComparisons / ht->p;
}

void freeHT1(HashTable1 *ht) {
    free(ht->data);
    free(ht);
}

// ==================== 拉链法 HT2 操作 ====================

HashTable2* initHashTable2(int size) {
    HashTable2 *ht = (HashTable2*)malloc(sizeof(HashTable2));
    ht->size = size;
    ht->count = 0;
    ht->p = getPrime(size);
    ht->data = (Node**)malloc(size * sizeof(Node*));
    for (int i = 0; i < size; i++) {
        ht->data[i] = NULL;
    }
    return ht;
}

// 插入并返回在链表中的位置（第几个插入到该链）
int insertHT2(HashTable2 *ht, int key, int *steps) {
    int addr = hash(key, ht->p);
    
    Node *cur = ht->data[addr];
    Node *tail = NULL;
    int pos = 1;
    while (cur != NULL) {
        if (cur->key == key) {
            *steps = pos;
            return addr;
        }
        tail = cur;
        cur = cur->next;
        pos++;
    }
    
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->next = NULL;
    
    if (tail == NULL) {
        ht->data[addr] = newNode;
    } else {
        tail->next = newNode;
    }
    ht->count++;
    *steps = pos;
    
    return addr;
}

Node* searchHT2(HashTable2 *ht, int key, int *comparisons) {
    int addr = hash(key, ht->p);
    *comparisons = 0;
    
    Node *cur = ht->data[addr];
    while (cur != NULL) {
        (*comparisons)++;
        if (cur->key == key) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

double calcASL_Success_HT2(HashTable2 *ht) {
    if (ht->count == 0) return 0;
    
    int totalComparisons = 0;
    for (int i = 0; i < ht->size; i++) {
        int pos = 1;
        Node *cur = ht->data[i];
        while (cur != NULL) {
            totalComparisons += pos;
            pos++;
            cur = cur->next;
        }
    }
    return (double)totalComparisons / ht->count;
}

double calcASL_Fail_HT2(HashTable2 *ht) {
    int totalComparisons = 0;
    
    for (int i = 0; i < ht->p; i++) {
        int len = 0;
        Node *cur = ht->data[i];
        while (cur != NULL) {
            len++;
            cur = cur->next;
        }
        totalComparisons += len;
    }
    return (double)totalComparisons / ht->p +1;
}

void freeHT2(HashTable2 *ht) {
    for (int i = 0; i < ht->size; i++) {
        Node *cur = ht->data[i];
        while (cur != NULL) {
            Node *temp = cur;
            cur = cur->next;
            free(temp);
        }
    }
    free(ht->data);
    free(ht);
}

// ==================== 主函数 ====================

int main() {
    int keys[MAX_SIZE];
    int insertSteps1[MAX_SIZE];  // 线性探测插入步数
    int insertSteps2[MAX_SIZE];  // 拉链法插入步数
    int n, tableSize;
    
    // 输入
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &keys[i]);
    }
    scanf("%d", &tableSize);
    
    // 构建哈希表
    HashTable1 *ht1 = initHashTable1(tableSize);
    HashTable2 *ht2 = initHashTable2(tableSize);
    
    for (int i = 0; i < n; i++) {
        insertHT1(ht1, keys[i], &insertSteps1[i]);
        insertHT2(ht2, keys[i], &insertSteps2[i]);
    }
    
    // ==================== 输出拉链法结果 ====================
    printf("# 拉链法\n");
    
    // 地址
    printf("地址： [");
    for (int i = 0; i < tableSize; i++) {
        printf("%d", i);
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 键值（链表形式）
    printf("键值： [");
    for (int i = 0; i < tableSize; i++) {
        printf("[");
        Node *cur = ht2->data[i];
        int first = 1;
        while (cur != NULL) {
            if (!first) printf(", ");
            printf("%d", cur->key);
            first = 0;
            cur = cur->next;
        }
        printf("]");
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 插入步数（链表形式）
    printf("插入步数： [");
    for (int i = 0; i < tableSize; i++) {
        printf("[");
        Node *cur = ht2->data[i];
        int pos = 1;
        int first = 1;
        while (cur != NULL) {
            if (!first) printf(", ");
            printf("%d", pos);
            first = 0;
            pos++;
            cur = cur->next;
        }
        printf("]");
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 查找步数（每个地址查找失败需要的比较次数）
    printf("查找步数： [");
    for (int i = 0; i < tableSize; i++) {
        int len = 0;
        Node *cur = ht2->data[i];
        while (cur != NULL) {
            len++;
            cur = cur->next;
        }
        printf("%d", len + 1);  // 查找失败需要遍历整个链表+1次空指针比较
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // ASL
    printf("成功平均长度： %.16g\n", calcASL_Success_HT2(ht2));
    printf("失败平均长度： %.16g\n", calcASL_Fail_HT2(ht2));
    
    // ==================== 输出线性探测法结果 ====================
    printf("\n# 线性探测法\n");
    
    // 地址
    printf("地址： [");
    for (int i = 0; i < tableSize; i++) {
        printf("%d", i);
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 键值
    printf("键值： [");
    for (int i = 0; i < tableSize; i++) {
        if (ht1->data[i] == EMPTY) {
            printf("None");
        } else {
            printf("%d", ht1->data[i]);
        }
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 插入步数
    printf("插入步数： [");
    for (int i = 0; i < tableSize; i++) {
        if (ht1->data[i] == EMPTY) {
            printf("0");
        } else {
            // 查找该位置元素的插入步数
            int key = ht1->data[i];
            int addr = hash(key, ht1->p);
            int steps = (i - addr + tableSize) % tableSize + 1;
            printf("%d", steps);
        }
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // 查找步数（从每个位置开始查找失败需要的比较次数）
    printf("查找步数： [");
    for (int i = 0; i < tableSize; i++) {
        int j = 0;
        while (ht1->data[(i + j) % tableSize] != EMPTY) {
            j++;
            if (j >= tableSize) break;
        }
        printf("%d", j + 1);
        if (i < tableSize - 1) printf(", ");
    }
    printf("]\n");
    
    // ASL
    printf("成功平均长度： %.16g\n", calcASL_Success_HT1(ht1));
    printf("失败平均长度： %.16g\n", calcASL_Fail_HT1(ht1));
    
    // 释放内存
    freeHT1(ht1);
    freeHT2(ht2);
    
    return 0;
}