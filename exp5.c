#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 200     // 哈希表最大容量（增大以支持更多测试）
#define EMPTY -1         // 空位标记
#define DELETED -2       // 删除标记（线性探测法）

// ==================== 线性探测法哈希表 HT1 ====================
typedef struct {
    int *data;           // 存储数据的数组
    int size;            // 哈希表大小
    int count;           // 当前元素个数
    int p;               // 除留余数法的除数
} HashTable1;

// ==================== 拉链法哈希表 HT2 ====================
typedef struct Node {
    int key;
    struct Node *next;
} Node;

typedef struct {
    Node **data;         // 链表头指针数组
    int size;            // 哈希表大小
    int count;           // 当前元素个数
    int p;               // 除留余数法的除数
} HashTable2;

// ==================== 辅助函数 ====================

// 判断是否为素数
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

// 获取不大于n的最大素数
int getPrime(int n) {
    while (n > 1 && !isPrime(n)) {
        n--;
    }
    return n;
}

// 哈希函数：除留余数法
int hash(int key, int p) {
    return key % p;
}

// ==================== 线性探测法 HT1 操作 ====================

// 初始化哈希表1
HashTable1* initHashTable1(int size) {
    HashTable1 *ht = (HashTable1*)malloc(sizeof(HashTable1));
    ht->size = size;
    ht->count = 0;
    ht->p = getPrime(size);  // 取不大于表长的最大素数
    ht->data = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        ht->data[i] = EMPTY;
    }
    printf("HT1: 表长=%d, 除数p=%d\n", size, ht->p);
    return ht;
}

// 在HT1中插入关键字
int insertHT1(HashTable1 *ht, int key) {
    if (ht->count >= ht->size) {
        printf("哈希表已满，无法插入 %d\n", key);
        return -1;
    }
    
    int addr = hash(key, ht->p);
    int i = 0;
    
    // 线性探测
    while (ht->data[(addr + i) % ht->size] != EMPTY && 
           ht->data[(addr + i) % ht->size] != DELETED) {
        // 检查是否已存在
        if (ht->data[(addr + i) % ht->size] == key) {
            return (addr + i) % ht->size;
        }
        i++;
        if (i >= ht->size) {
            printf("哈希表已满，无法插入 %d\n", key);
            return -1;
        }
    }
    
    int pos = (addr + i) % ht->size;
    ht->data[pos] = key;
    ht->count++;
    return pos;
}

// 在HT1中查找关键字，返回比较次数
int searchHT1(HashTable1 *ht, int key, int *comparisons) {
    int addr = hash(key, ht->p);
    int i = 0;
    *comparisons = 0;
    
    while (i < ht->size) {
        int pos = (addr + i) % ht->size;
        (*comparisons)++;
        
        if (ht->data[pos] == EMPTY) {
            return -1;  // 未找到
        }
        if (ht->data[pos] == key) {
            return pos;  // 找到
        }
        i++;
    }
    return -1;  // 未找到
}

// 计算HT1查找成功的ASL
double calcASL_Success_HT1(HashTable1 *ht) {
    if (ht->count == 0) return 0;
    
    int totalComparisons = 0;
    
    for (int i = 0; i < ht->size; i++) {
        if (ht->data[i] != EMPTY && ht->data[i] != DELETED) {
            int key = ht->data[i];
            int comparisons;
            searchHT1(ht, key, &comparisons);
            totalComparisons += comparisons;
        }
    }
    
    return (double)totalComparisons / ht->count;
}

// 计算HT1查找失败的ASL
double calcASL_Fail_HT1(HashTable1 *ht) {
    int totalComparisons = 0;
    
    // 对于每个可能的哈希地址(0到p-1)，计算查找失败需要的比较次数
    for (int i = 0; i < ht->p; i++) {
        int j = 0;
        // 从地址i开始探测，直到遇到空位置
        while (ht->data[(i + j) % ht->size] != EMPTY) {
            j++;
            if (j >= ht->size) break;  // 防止无限循环
        }
        totalComparisons += (j + 1);  // 包括与空位置的比较
    }
    
    return (double)totalComparisons / ht->p;
}

// 打印HT1
void printHT1(HashTable1 *ht) {
    printf("\n========== 哈希表HT1（线性探测法）==========\n");
    printf("地址:   ");
    for (int i = 0; i < ht->size; i++) {
        printf("%4d ", i);
    }
    printf("\n关键字: ");
    for (int i = 0; i < ht->size; i++) {
        if (ht->data[i] == EMPTY) {
            printf("   - ");
        } else {
            printf("%4d ", ht->data[i]);
        }
    }
    printf("\n");
}

// 释放HT1
void freeHT1(HashTable1 *ht) {
    free(ht->data);
    free(ht);
}

// ==================== 拉链法 HT2 操作 ====================

// 初始化哈希表2
HashTable2* initHashTable2(int size) {
    HashTable2 *ht = (HashTable2*)malloc(sizeof(HashTable2));
    ht->size = size;
    ht->count = 0;
    ht->p = getPrime(size);
    ht->data = (Node**)malloc(size * sizeof(Node*));
    for (int i = 0; i < size; i++) {
        ht->data[i] = NULL;
    }
    printf("HT2: 表长=%d, 除数p=%d\n", size, ht->p);
    return ht;
}

// 在HT2中插入关键字（尾插法，保持插入顺序）
int insertHT2(HashTable2 *ht, int key) {
    int addr = hash(key, ht->p);
    
    // 检查是否已存在
    Node *cur = ht->data[addr];
    Node *tail = NULL;
    while (cur != NULL) {
        if (cur->key == key) {
            return addr;  // 已存在
        }
        tail = cur;
        cur = cur->next;
    }
    
    // 尾插法插入新节点
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->next = NULL;
    
    if (tail == NULL) {
        ht->data[addr] = newNode;  // 链表为空
    } else {
        tail->next = newNode;      // 插入到尾部
    }
    ht->count++;
    
    return addr;
}

// 在HT2中查找关键字，返回比较次数
Node* searchHT2(HashTable2 *ht, int key, int *comparisons) {
    int addr = hash(key, ht->p);
    *comparisons = 0;
    
    Node *cur = ht->data[addr];
    while (cur != NULL) {
        (*comparisons)++;
        if (cur->key == key) {
            return cur;  // 找到
        }
        cur = cur->next;
    }
    
    // 查找失败时，如果链表为空，比较次数为0（只是访问空指针）
    // 如果链表非空，比较次数为链表长度
    return NULL;  // 未找到
}

// 计算HT2查找成功的ASL
double calcASL_Success_HT2(HashTable2 *ht) {
    if (ht->count == 0) return 0;
    
    int totalComparisons = 0;
    
    for (int i = 0; i < ht->size; i++) {
        int pos = 1;  // 链表中的位置
        Node *cur = ht->data[i];
        while (cur != NULL) {
            totalComparisons += pos;
            pos++;
            cur = cur->next;
        }
    }
    
    return (double)totalComparisons / ht->count;
}

// 计算HT2查找失败的ASL
double calcASL_Fail_HT2(HashTable2 *ht) {
    int totalComparisons = 0;
    
    // 对于每个哈希地址(0到p-1)，计算链表长度
    for (int i = 0; i < ht->p; i++) {
        int len = 0;
        Node *cur = ht->data[i];
        while (cur != NULL) {
            len++;
            cur = cur->next;
        }
        totalComparisons += len;  // 查找失败需要比较len次（遍历整个链表）
    }
    
    return (double)totalComparisons / ht->p;
}

// 打印HT2
void printHT2(HashTable2 *ht) {
    printf("\n========== 哈希表HT2（拉链法）==========\n");
    for (int i = 0; i < ht->size; i++) {
        printf("地址 %2d: ", i);
        Node *cur = ht->data[i];
        if (cur == NULL) {
            printf("NULL");
        }
        while (cur != NULL) {
            printf("%d", cur->key);
            if (cur->next != NULL) {
                printf(" -> ");
            }
            cur = cur->next;
        }
        printf("\n");
    }
}

// 释放HT2
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
    int n, tableSize;
    
    // 1. 输入关键字序列
    printf("====== 哈希表实验 ======\n");
    printf("请输入关键字个数: ");
    scanf("%d", &n);
    
    printf("请输入%d个关键字: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &keys[i]);
    }
    
    printf("请输入哈希表大小: ");
    scanf("%d", &tableSize);
    
    // 2.  用除留余数法构建哈希函数，用线性探测法解决冲突，构建哈希表HT1
    printf("\n>>> 构建哈希表HT1（线性探测法）.. .\n");
    HashTable1 *ht1 = initHashTable1(tableSize);
    
    printf("插入过程:\n");
    for (int i = 0; i < n; i++) {
        int addr = hash(keys[i], ht1->p);
        int pos = insertHT1(ht1, keys[i]);
        printf("  插入 %d: H(%d)=%d", keys[i], keys[i], addr);
        if (pos != addr) {
            printf(", 冲突, 最终位置=%d", pos);
        }
        printf("\n");
    }
    printHT1(ht1);
    
    // 3. 用除留余数法构建哈希函数，用拉链法解决冲突，构建哈希表HT2
    printf("\n>>> 构建哈希表HT2（拉链法）.. .\n");
    HashTable2 *ht2 = initHashTable2(tableSize);
    
    printf("插入过程:\n");
    for (int i = 0; i < n; i++) {
        int addr = insertHT2(ht2, keys[i]);
        printf("  插入 %d: H(%d)=%d\n", keys[i], keys[i], addr);
    }
    printHT2(ht2);
    
    // 4.  分别对HT1和HT2计算在等概率情况下查找成功和查找失败的ASL
    printf("\n========== ASL计算结果 ==========\n");
    
    double asl_success_ht1 = calcASL_Success_HT1(ht1);
    double asl_fail_ht1 = calcASL_Fail_HT1(ht1);
    printf("HT1（线性探测法）:\n");
    printf("  查找成功的ASL = %.4f\n", asl_success_ht1);
    printf("  查找失败的ASL = %.4f\n", asl_fail_ht1);
    
    double asl_success_ht2 = calcASL_Success_HT2(ht2);
    double asl_fail_ht2 = calcASL_Fail_HT2(ht2);
    printf("HT2（拉链法）:\n");
    printf("  查找成功的ASL = %.4f\n", asl_success_ht2);
    printf("  查找失败的ASL = %.4f\n", asl_fail_ht2);
    
    // 5. 分别在HT1和HT2中查找给定的关键字，给出比较次数
    printf("\n========== 查找测试 ==========\n");
    int searchKey;
    char choice;
    
    do {
        printf("请输入要查找的关键字: ");
        scanf("%d", &searchKey);
        
        int comparisons1, comparisons2;
        
        // 在HT1中查找
        int pos1 = searchHT1(ht1, searchKey, &comparisons1);
        printf("\nHT1查找结果: ");
        if (pos1 != -1) {
            printf("找到!  位置=%d, 比较次数=%d\n", pos1, comparisons1);
        } else {
            printf("未找到!  比较次数=%d\n", comparisons1);
        }
        
        // 在HT2中查找
        Node *node = searchHT2(ht2, searchKey, &comparisons2);
        printf("HT2查找结果: ");
        if (node != NULL) {
            printf("找到!  比较次数=%d\n", comparisons2);
        } else {
            printf("未找到!  比较次数=%d\n", comparisons2);
        }
        
        printf("\n继续查找? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');
    
    // 释放内存
    freeHT1(ht1);
    freeHT2(ht2);
    
    printf("\n程序结束。\n");
    return 0;
}