#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define OK 1
#define ERROR 0
#define MAXSIZE 10000
typedef int Status;

typedef struct{
    unsigned int weight;
    unsigned int parent, lchild, rchild;
}HTNode, *HuffmanTree;
typedef char** HuffmanCode;

void CreateHuffmanTree(HuffmanTree *HT,int n){
    if(n <= 1) return;
    int m = 2 * n - 1;
    *HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    for(int i = 1; i <= n; i++){
        (*HT)[i].weight = 0;
        (*HT)[i].parent = 0;
        (*HT)[i].lchild = 0;
        (*HT)[i].rchild = 0;
    }
    for(int i = n + 1; i <= m; i++){
        int min1, min2;
        min1 = min2 = INT_MAX;
        int x1 = 0, x2 = 0;
        for(int j = 1; j < i; j++){
            if((*HT)[j].parent == 0){
                if((*HT)[j].weight < min1){
                    min2 = min1;
                    x2 = x1;
                    min1 = (*HT)[j].weight;
                    x1 = j;
                } else if((*HT)[j].weight < min2){
                    min2 = (*HT)[j].weight;
                    x2 = j;
                }
            }
        }
        (*HT)[x1].parent = i;
        (*HT)[x2].parent = i;
        (*HT)[i].lchild = x1;
        (*HT)[i].rchild = x2;
        (*HT)[i].weight = (*HT)[x1].weight + (*HT)[x2].weight;
    }

}
Status GetFrequency(char str[], char list[], char freq[], int len){
    int i, j, k;
    int found;
    int list_len = 0;
    for(i = 0; i < len; i++){
        found = 0;
        for(j = 0; j < list_len; j++){
            if(str[i] == list[j]){
                freq[j]++;
                found = 1;
                break;
            }
        }
        if(!found){
            list[list_len] = str[i];
            freq[list_len] = 1;
            list_len++;
        }
    }
    // 输出结果
    printf("字符\t频率\n");
    for(k = 0; k < list_len; k++){
        printf("%c\t%d\n", list[k], freq[k]);
    }
    return OK;
}
int main(){
    char str[MAXSIZE];
    char list[500];
    char freq[500];
    printf("输入电文字符串：\n");
    fgets(str, MAXSIZE, stdin);
    int len = strlen(str);


    return 0;
}