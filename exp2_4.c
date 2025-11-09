#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PROCESS_TIME 10    // 默认客户交易时长
#define MAX_QUEUE_SIZE 100        // 队列最大容量

// 定义事件结构体
typedef struct {
    int time;          // 事件发生时间
    int customerIndex; // 客户编号
    int amount;        // 交易金额
    int processTime;   // 处理时间
} Event;

// 定义客户结构体
typedef struct {
    int customerIndex;  // 客户编号
    int amount;         // 交易金额
    int arriveTime;     // 到达时间
    int waitTime;       // 等待时间
    int processTime;    // 处理时间
} Customer;

// 定义队列结构体
typedef struct {
    Customer* data;     // 存储数据的数组
    int front;          // 队头指针
    int rear;          // 队尾指针
    int size;          // 当前队列大小
} Queue;

// 定义事件队列
typedef struct {
    Event* events;      // 事件数组
    int count;          // 事件数量
} EventQueue;

// 初始化队列
Queue* initQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->data = (Customer*)malloc(sizeof(Customer) * MAX_QUEUE_SIZE);
    q->front = q->rear = 0;
    q->size = 0;
    return q;
}

// 入队
int enqueue(Queue* q, Customer c) {
    if ((q->rear + 1) % MAX_QUEUE_SIZE == q->front) return 0; // 队列已满
    q->data[q->rear] = c;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->size++;
    return 1;
}

// 出队
int dequeue(Queue* q, Customer* c) {
    if (q->front == q->rear) return 0; // 队列为空
    *c = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return 1;
}

// 判断队列是否为空
int isEmpty(Queue* q) {
    return q->front == q->rear;
}

// 事件队列操作
void addEvent(EventQueue* eq, int time, int customerIndex, int amount, int processTime) {
    Event e = {time, customerIndex, amount, processTime};
    // 找到合适的插入位置（按时间排序）
    int i = eq->count - 1;
    while (i >= 0 && eq->events[i].time > time) {
        eq->events[i + 1] = eq->events[i];
        i--;
    }
    eq->events[i + 1] = e;
    eq->count++;
}

// 银行模拟函数
void bankSimulation(int total, int closeTime, int N, int* transactions, int* arriveTimes) {
    Queue* queue2 = initQueue();  // 取款业务队列（只需要一个队列，存款可以直接处理）
    
    // 初始化事件队列
    EventQueue eq;
    eq.events = (Event*)malloc(sizeof(Event) * N); // 每个客户只有一个事件
    eq.count = 0;
    
    int currentTime = 0;          // 当前时间
    int totalWaitTime = 0;        // 总等待时间
    int processedCustomers = 0;   // 已处理的客户数
    int bank_balance = total;     // 当前银行余额
    int* customerWaitTimes = (int*)calloc(N, sizeof(int)); // 存储每个客户的等待时间
    
    // 初始化所有事件
    for (int i = 0; i < N; i++) {
        addEvent(&eq, arriveTimes[i], i, transactions[i], DEFAULT_PROCESS_TIME);
    }
    
    // 处理所有事件
    while (eq.count > 0 && currentTime <= closeTime) {
        // 获取下一个事件
        Event currentEvent = eq.events[0];
        
        // 更新当前时间
        currentTime = currentEvent.time;
        
        // 移除当前事件
        for (int i = 0; i < eq.count - 1; i++) {
            eq.events[i] = eq.events[i + 1];
        }
        eq.count--;
        
        // 处理事件
        if (arriveTimes[currentEvent.customerIndex] <= closeTime) {
            Customer c;
            c.customerIndex = currentEvent.customerIndex;
            c.amount = currentEvent.amount;
            c.arriveTime = arriveTimes[currentEvent.customerIndex];
            c.processTime = currentEvent.processTime;
            c.waitTime = 0;
            
            if (c.amount > 0) {
                // 存款业务直接处理
                bank_balance += c.amount;
                c.waitTime = currentTime - c.arriveTime;
                if (c.waitTime < 0) c.waitTime = 0;
                customerWaitTimes[currentEvent.customerIndex] = c.waitTime;
                totalWaitTime += c.waitTime;
                processedCustomers++;
            } else {
                // 取款业务需要检查余额
                if (bank_balance >= -c.amount) {
                    bank_balance += c.amount;
                    c.waitTime = currentTime - c.arriveTime;
                    if (c.waitTime < 0) c.waitTime = 0;
                    customerWaitTimes[currentEvent.customerIndex] = c.waitTime;
                    totalWaitTime += c.waitTime;
                    processedCustomers++;
                } else {
                    // 余额不足，加入取款队列
                    enqueue(queue2, c);
                }
            }
        }

    }
    
    // 处理余下的取款队列
    while (!isEmpty(queue2) && currentTime <= closeTime) {
        Customer c;
        if (dequeue(queue2, &c)) {
            if (bank_balance >= -c.amount) {
                bank_balance += c.amount;
                c.waitTime = currentTime - c.arriveTime;
                if (c.waitTime < 0) c.waitTime = 0;
                customerWaitTimes[c.customerIndex] = c.waitTime;
                totalWaitTime += c.waitTime;
                processedCustomers++;
                currentTime += c.processTime;
            } else {
                // 余额不足，将客户重新排队到队尾
                enqueue(queue2, c);
                currentTime++; // 增加一个时间单位，避免死循环
            }
        }
    }

    // 输出每个客户的等待时间和平均等待时间
    printf("各客户等待时间：\n");
    for (int i = 0; i < N; i++) {
        printf("客户 %d: %d分钟\n", i + 1, customerWaitTimes[i]);
    }
    printf("\n平均等待时间：");
    if (processedCustomers > 0) {
        printf("%d分钟\n", totalWaitTime / processedCustomers);
    } else {
        printf("0分钟\n");
    }

    // 释放内存
    free(eq.events);
    free(queue2->data);
    free(queue2);
    free(customerWaitTimes);
}

int main(){
    int N, total, closeTime;
    printf("请输入客户数量、初始总金额和银行关闭时间（以空格分隔）：\n");
    scanf("%d %d %d", &N, &total, &closeTime);

    int* transactions = (int*)malloc(N * sizeof(int));
    int* arriveTimes = (int*)malloc(N * sizeof(int));

    // 读取每个客户的交易金额和到达时间
    for (int i = 0; i < N; i++) {
        printf("请输入第 %d 个客户的交易金额和到达时间（以空格分隔）：\n", i + 1);
        scanf("%d %d", &transactions[i], &arriveTimes[i]);
    }

    // 调用银行模拟函数
    bankSimulation(total, closeTime, N, transactions, arriveTimes);

    // 释放内存
    free(transactions);
    free(arriveTimes);
    return 0;
}

