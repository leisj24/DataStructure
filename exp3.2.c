#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<sys/stat.h>

#define OK 1
#define ERROR 0
#define MAXSIZE 100000

typedef struct{
    unsigned int weight;
    unsigned int parent, lchild, rchild;
}HTNode, *HuffmanTree;
typedef char** HuffmanCode;

// ==================== Huffman树相关函数 ====================

void CreateHuffmanTree(HuffmanTree *HT, int n){
    if(n <= 1) return;
    int m = 2 * n - 1;
    
    for(int i = 1; i <= m; i++){
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

void HuffmanCoding(HuffmanTree HT, HuffmanCode *HC, int n){
    *HC = (HuffmanCode)malloc((n + 1) * sizeof(char*));
    char *cd = (char*)malloc(n * sizeof(char));
    cd[n - 1] = '\0';
    
    int i, start, c, f;
    for(i = 1; i <= n; ++i){
        start = n - 1;
        c = i;
        f = HT[i].parent;
        
        while(f != 0){
            --start;
            if(HT[f].lchild == c) cd[start] = '0';
            else cd[start] = '1';
            c = f;
            f = HT[f].parent;
        }
        
        (*HC)[i] = (char*)malloc((n - start) * sizeof(char));
        strcpy((*HC)[i], &cd[start]);
    }
    free(cd);
}

// ==================== 文件压缩相关函数 ====================

// 统计文件中所有字节的频率
int GetByteFrequency(FILE *fp, unsigned int freq[256]){
    unsigned char byte;
    int unique_bytes = 0;
    
    memset(freq, 0, sizeof(unsigned int) * 256);
    
    while(fread(&byte, 1, 1, fp) == 1){
        if(freq[byte] == 0){
            unique_bytes++;
        }
        freq[byte]++;
    }
    
    rewind(fp);
    return unique_bytes;
}

// 建立字节到索引的映射表
void BuildByteMap(unsigned int freq[256], unsigned char byteMap[256], int n){
    int index = 0;
    for(int i = 0; i < 256; i++){
        if(freq[i] > 0){
            byteMap[i] = index++;
        }
    }
}

// 将二进制字符串写入文件（打包成字节）
void WriteBitStream(FILE *outfp, const char *bitStream){
    int len = strlen(bitStream);
    unsigned char byte = 0;
    int bitPos = 0;
    
    for(int i = 0; i < len; i++){
        byte |= (unsigned char)((bitStream[i] == '1' ? 1 : 0) << (7 - bitPos));
        bitPos++;
        
        if(bitPos == 8){
            fwrite(&byte, 1, 1, outfp);
            byte = 0;
            bitPos = 0;
        }
    }
    
    // 写入最后的字节（可能不足8位）
    if(bitPos > 0){
        fwrite(&byte, 1, 1, outfp);
    }
}

// 压缩文件函数
void CompressFile(const char *inputFile, const char *outputFile){
    FILE *infp = fopen(inputFile, "rb");
    if(!infp){
        printf("错误：无法打开输入文件 %s\n", inputFile);
        return;
    }
    
    FILE *outfp = fopen(outputFile, "wb");
    if(!outfp){
        printf("错误：无法创建输出文件 %s\n", outputFile);
        fclose(infp);
        return;
    }
    
    // 第1步：统计字节频率
    unsigned int freq[256];
    int n = GetByteFrequency(infp, freq);
    
    if(n == 0){
        printf("文件为空\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    printf("不同字节数: %d\n", n);
    
    // 第2步：创建Huffman树
    int m = 2 * n - 1;
    HuffmanTree HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    
    int index = 0;
    for(int i = 0; i < 256; i++){
        if(freq[i] > 0){
            HT[++index].weight = freq[i];
        }
    }
    
    CreateHuffmanTree(&HT, n);
    
    // 第3步：生成Huffman编码
    HuffmanCode HC;
    HuffmanCoding(HT, &HC, n);
    
    // 第4步：写入压缩文件头部信息
    // 格式：不同字节数(1字节) + 字节值(n字节) + 频率值(4n字节)
    unsigned char uniqueByteCount = (unsigned char)n;
    fwrite(&uniqueByteCount, 1, 1, outfp);
    
    int byteIndex = 0;
    unsigned char byteList[256];
    unsigned int freqList[256];
    
    for(int i = 0; i < 256; i++){
        if(freq[i] > 0){
            byteList[byteIndex] = (unsigned char)i;
            freqList[byteIndex] = freq[i];
            byteIndex++;
        }
    }
    
    fwrite(byteList, 1, n, outfp);
    fwrite(freqList, sizeof(unsigned int), n, outfp);
    
    // 第5步：读取文件并进行编码
    unsigned char byte;
    char bitStream[10000] = {0};
    int bitStreamLen = 0;
    
    while(fread(&byte, 1, 1, infp) == 1){
        int byteIdx = -1;
        for(int i = 0; i < n; i++){
            if(byteList[i] == byte){
                byteIdx = i;
                break;
            }
        }
        
        if(byteIdx != -1){
            strcat(bitStream, HC[byteIdx + 1]);
            bitStreamLen = strlen(bitStream);
            
            // 每积累一定量的比特流就写入文件
            if(bitStreamLen >= 8000){
                WriteBitStream(outfp, bitStream);
                memset(bitStream, 0, sizeof(bitStream));
                bitStreamLen = 0;
            }
        }
    }
    
    // 写入剩余的比特流
    if(bitStreamLen > 0){
        WriteBitStream(outfp, bitStream);
    }
    
    printf("压缩完成\n");
    
    // 释放资源
    for(int i = 1; i <= n; i++){
        if(HC[i]) free(HC[i]);
    }
    free(HC);
    free(HT);
    
    fclose(infp);
    fclose(outfp);
}

// ==================== 文件解压缩相关函数 ====================

// 从文件读取比特流
int ReadBitStream(FILE *infp, char *bitStream, int maxLen){
    unsigned char byte;
    int bitPos = 0;
    
    while(fread(&byte, 1, 1, infp) == 1 && bitPos < maxLen){
        for(int i = 7; i >= 0 && bitPos < maxLen; i--){
            bitStream[bitPos++] = ((byte >> i) & 1) ? '1' : '0';
        }
    }
    bitStream[bitPos] = '\0';
    return bitPos;
}

// 解压缩文件函数
void DecompressFile(const char *inputFile, const char *outputFile){
    FILE *infp = fopen(inputFile, "rb");
    if(!infp){
        printf("错误：无法打开压缩文件 %s\n", inputFile);
        return;
    }
    
    FILE *outfp = fopen(outputFile, "wb");
    if(!outfp){
        printf("错误：无法创建输出文件 %s\n", outputFile);
        fclose(infp);
        return;
    }
    
    // 第1步：读取压缩文件头部信息
    unsigned char uniqueByteCount;
    if(fread(&uniqueByteCount, 1, 1, infp) != 1){
        printf("错误：无法读取文件头\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    int n = uniqueByteCount;
    
    unsigned char byteList[256];
    unsigned int freqList[256];
    
    if(fread(byteList, 1, n, infp) != n){
        printf("错误：无法读取字节表\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    if(fread(freqList, sizeof(unsigned int), n, infp) != n){
        printf("错误：无法读取频率表\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    printf("不同字节数: %d\n", n);
    
    // 第2步：重建Huffman树
    int m = 2 * n - 1;
    HuffmanTree HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    
    for(int i = 0; i < n; i++){
        HT[i + 1].weight = freqList[i];
    }
    
    CreateHuffmanTree(&HT, n);
    
    // 第3步：读取比特流并解码
    char bitStream[10000];
    int bitStreamLen = ReadBitStream(infp, bitStream, sizeof(bitStream) - 1);
    
    int root = 2 * n - 1;
    int current = root;
    int decodedCount = 0;
    
    for(int i = 0; i < bitStreamLen; i++){
        if(bitStream[i] == '0'){
            current = HT[current].lchild;
        } else if(bitStream[i] == '1'){
            current = HT[current].rchild;
        }
        
        // 到达叶子节点
        if(HT[current].lchild == 0 && HT[current].rchild == 0){
            unsigned char byte = byteList[current - 1];
            fwrite(&byte, 1, 1, outfp);
            decodedCount++;
            current = root;
        }
    }
    
    printf("解压完成，解压字节数: %d\n", decodedCount);
    
    // 释放资源
    free(HT);
    
    fclose(infp);
    fclose(outfp);
}


int main(){
    int choice;
    char inputFile[256];
    char outputFile[256];
    
    while(1){
        printf("\n========== Huffman文件压缩程序 ==========\n");
        printf("1. 压缩文件\n");
        printf("2. 解压缩文件\n");
        printf("3. 退出\n");
        printf("请选择操作 (1-3): ");
        scanf("%d", &choice);
        getchar();  // 清除换行符
        
        switch(choice){
            case 1:
                printf("输入要压缩的文件名 (同目录): ");
                fgets(inputFile, sizeof(inputFile), stdin);
                inputFile[strcspn(inputFile, "\n")] = 0;  // 移除换行符
                
                // 自动生成输出文件名
                snprintf(outputFile, sizeof(outputFile), "%s.huff", inputFile);
                
                printf("输入文件: %s\n", inputFile);
                printf("输出文件: %s\n", outputFile);
                printf("开始压缩...\n");
                CompressFile(inputFile, outputFile);
                break;
                
            case 2:
                printf("输入要解压的.huff文件名 (同目录): ");
                fgets(inputFile, sizeof(inputFile), stdin);
                inputFile[strcspn(inputFile, "\n")] = 0;
                
                // 自动生成输出文件名（去掉.huff扩展名）
                strcpy(outputFile, inputFile);
                char *dot = strrchr(outputFile, '.');
                if(dot && strcmp(dot, ".huff") == 0){
                    *dot = '\0';
                } else {
                    strcat(outputFile, ".extracted");
                }
                
                printf("输入文件: %s\n", inputFile);
                printf("输出文件: %s\n", outputFile);
                printf("开始解压...\n");
                DecompressFile(inputFile, outputFile);
                break;
                
            case 3:
                printf("程序退出\n");
                return 0;
                
            default:
                printf("错误：请输入有效的选项\n");
        }
    }
    
    return 0;
}
