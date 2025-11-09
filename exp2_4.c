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
    Queue* queue1 = initQueue();  // 存款队列
    Queue* queue2 = initQueue();  // 取款队列
    
    // 初始化事件队列
    EventQueue eq;
    eq.events = (Event*)malloc(sizeof(Event) * N); // 每个客户只有一个事件
    eq.count = 0;
    
    int currentTime = 0;          // 当前时间
    int totalWaitTime = 0;        // 总等待时间
    int processedCustomers = 0;   // 已处理的客户数
    int bank_balance = total;     // 当前银行余额
    int* customerWaitTimes = (int*)calloc(N, sizeof(int)); // 存储每个客户的等待时间
    int* lastUpdateTime = (int*)calloc(N, sizeof(int));    // 记录每个客户最后一次更新等待时间的时刻
    
    // 初始化lastUpdateTime为到达时间
    for(int i = 0; i < N; i++) {
        lastUpdateTime[i] = arriveTimes[i];
    }
    
    // 初始化所有事件
    for (int i = 0; i < N; i++) {
        addEvent(&eq, arriveTimes[i], i, transactions[i], DEFAULT_PROCESS_TIME);
    }
    
    // 处理所有到达事件（按时间顺序），并在到达间隙处理队列中的客户
    while (eq.count > 0 && currentTime <= closeTime) {
        // 获取下一个到达事件
        Event currentEvent = eq.events[0];
        int nextEventTime = (eq.count > 1) ? eq.events[1].time : 0x7FFFFFFF;

        // 更新当前时间到该到达时间（如果还没到）
        if (currentEvent.time > currentTime) currentTime = currentEvent.time;

        // 移除当前事件
        for (int i = 0; i < eq.count - 1; i++) {
            eq.events[i] = eq.events[i + 1];
        }
        eq.count--;

        // 创建客户并按类型入队
        if (currentEvent.time <= closeTime) {
            Customer c;
            c.customerIndex = currentEvent.customerIndex;
            c.amount = currentEvent.amount;
            c.arriveTime = currentEvent.time;
            c.processTime = currentEvent.processTime;
            c.waitTime = 0;

            if (c.amount > 0) {
                enqueue(queue1, c); // 存款队列
            } else {
                enqueue(queue2, c); // 取款队列
            }
        }

        // 在下一个到达时间之前尽可能处理队列中的客户
        while (( !isEmpty(queue1) || !isEmpty(queue2) ) && currentTime <= closeTime) {
            Customer *f1 = NULL, *f2 = NULL;
            if (!isEmpty(queue1)) f1 = &queue1->data[queue1->front];
            if (!isEmpty(queue2)) f2 = &queue2->data[queue2->front];

            // 决定处理哪个队列
            Customer cur;
            int processed = 0;

            // 优先检查取款是否可行
            if (f2 && f2->arriveTime <= currentTime) {
                if (bank_balance >= -f2->amount) {
                    dequeue(queue2, &cur);
                    processed = 1;
                } else if (f1 && f1->arriveTime <= currentTime) {
                    // 取款无法进行，但有存款可以处理
                    dequeue(queue1, &cur);
                    processed = 1;
                } else {
                    // 取款无法进行且无存款，等待下一笔存款
                    currentTime = nextEventTime;
                    break;
                }
            } else if (f1 && f1->arriveTime <= currentTime) {
                // 处理存款
                dequeue(queue1, &cur);
                processed = 1;
            } else {
                // 没有可以立即处理的客户
                currentTime = (f1 && f2) ? 
                    (f1->arriveTime < f2->arriveTime ? f1->arriveTime : f2->arriveTime) :
                    (f1 ? f1->arriveTime : (f2 ? f2->arriveTime : nextEventTime));
                break;
            }
            
            if (processed) {
                // 计算开始处理时间和等待时间
                int startTime = (currentTime > cur.arriveTime) ? currentTime : cur.arriveTime;
                if (startTime > closeTime) {
                    // 超过营业时间，不再处理
                    break;
                }

            // 计算等待时间
            if (cur.amount < 0 && lastUpdateTime[cur.customerIndex] < currentTime) {
                // 取款客户的等待时间包括从最后更新时间到当前时间的等待
                customerWaitTimes[cur.customerIndex] += currentTime - lastUpdateTime[cur.customerIndex];
            }
            
            // 更新最后处理时间
            lastUpdateTime[cur.customerIndex] = currentTime;
            processedCustomers++;
            
            // 完成交易
            bank_balance += cur.amount;
            currentTime += cur.processTime;
            
            // 更新所有在队列中取款客户的等待时间
            if (!isEmpty(queue2)) {
                Customer* nextWithdraw = &queue2->data[queue2->front];
                if (bank_balance < -nextWithdraw->amount) {
                    // 如果下一个取款客户无法处理，增加等待时间
                    customerWaitTimes[nextWithdraw->customerIndex] += cur.processTime;
                }
            }                // 完成交易，更新余额与当前时间
                bank_balance += cur.amount;
                currentTime = startTime + cur.processTime;
                
                // 更新处理完成后的lastUpdateTime
                lastUpdateTime[cur.customerIndex] = currentTime;
            }

            // 如果处理时间已经到达或超过下一个到达事件时间，返回到主循环，处理新的到达
            if (currentTime >= nextEventTime) break;
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

    // Output waiting times for each customer
    printf("每位客户等待时间:\n");
    for (int i = 0; i < N; i++) {
        printf("客户 %d: %d 分钟\n", 
               i + 1, transactions[i]);
    }
    
    printf("平均等待时间: ");
    if (processedCustomers > 0) {
        printf("%d 分钟\n", totalWaitTime / processedCustomers);
    } else {
        printf("0 分钟\n");
    }

    // 释放内存
    free(eq.events);
    free(queue1->data);
    free(queue1);
    free(queue2->data);
    free(queue2);
    free(customerWaitTimes);
    free(lastUpdateTime);
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

