#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

//TODO ：windows系统用这个函数 sleep(2) ->> Sleep(2000)
//#include <windows.h>

#define N_product 5   // 生产者的数目
#define N_prochase 5   // 消费者的数目
#define sleep_product 2 //生产者睡眠时间
#define sleep_prochase 5 //消费者睡眠时间
#define M 50 // 缓冲数目
int in = 0; // 生产者放置产品的位置
int out = 0; // 消费者取产品的位置

int buff[M] = { 0 }; // 缓冲初始化为0，开始时没有产品


sem_t empty_sem; // 同步信号量，当满了时阻止生产者放产品
sem_t full_sem; // 同步信号量，当没产品时阻止消费者消费

pthread_mutex_t mutex_product; // 互斥信号量，一次只有一个线程访问缓冲
pthread_mutex_t mutex_prochase; // 互斥信号量，一次只有一个线程访问缓冲

int product_id = 0; //生产者id
int prochase_id = 0; //消费者id
//信号处理函数
void Handlesignal(int signo){
    printf("程序退出\n",signo);
    exit(0);
}
/* 打印缓冲情况 */
void print() {
    int i;
    printf("产品队列为");
    char temp[M+1];
    for(i = 0; i < M; i++)
        if(buff[i]){
            temp[i]='*';
        }else{
            temp[i]='.';
        }
    temp[i]='\0';
    printf("%s\n",temp);
    fflush(stdout);
}

/* 生产者方法 */
void *product() {
    int id = ++product_id;
    while(1) {//重复进行
        //用sleep的数量可以调节生产和消费的速度，便于观察
        sleep(sleep_product);

        sem_wait(&empty_sem);
        pthread_mutex_lock(&mutex_product);

        in= in % M;
        printf(" + 生产者%d在产品队列中放入第%d个产品\t",id, in);
        fflush(stdout);

        buff[in]= 1;
        print();
        ++in;

        pthread_mutex_unlock(&mutex_product);
        sem_post(&full_sem);
    }
}

/* 消费者方法 */
void *prochase() {
    int id = ++prochase_id;
    while(1) {//重复进行
        //用sleep的数量可以调节生产和消费的速度，便于观察
        sleep(sleep_prochase);

        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex_prochase);

        out= out % M;
        printf(" - 消费者%d从产品队列中取出第%d个产品\t",id, out);
        fflush(stdout);

        buff[out]= 0;
        print();
        ++out;

        pthread_mutex_unlock(&mutex_prochase);
        sem_post(&empty_sem);
    }
}

int main() {
    printf("生产者%d个和消费者%d个,"
            "产品缓冲为%d,生产者每%d秒生产一个产品，"
            "消费者每%d秒消费一个产品,Ctrl+退出程序\n",
             N_product,N_prochase,M,sleep_product,sleep_prochase);
    pthread_t id1[N_prochase];
    pthread_t id2[N_product];
    int i;
    int ret[N_prochase+N_product];
    //结束程序
    if(signal(SIGINT,Handlesignal)==SIG_ERR){//按ctrl+C产生SIGINT信号
        printf("信号安装出错\n");
    }
//TODO: 初始化同步信号量
    int ini1 = sem_init(&empty_sem, 0, M);//产品队列缓冲同步
    int ini2 = sem_init(&full_sem, 0, 0);//线程运行同步
    if(ini1 && ini2 != 0) {
        printf("信号量初始化失败！\n");
        exit(1);
    }
//TODO: 初始化互斥信号量
    int ini3 = pthread_mutex_init(&mutex_product, NULL);
    int ini4 = pthread_mutex_init(&mutex_prochase, NULL);
    if(ini3 != 0 ||ini4!=0) {
        printf("线程同步初始化失败！\n");
        exit(1);
    }
//TODO: 创建N个生产者线程
    for(i = 0; i < N_product; i++) {
        ret[i]= pthread_create(&id1[i], NULL, product, (void *) (&i));
        if(ret[i] != 0) {
            printf("生产者%d线程创建失败！\n", i);
            exit(1);
        }
    }
//TODO: 创建N个消费者线程
    for(i = 0; i < N_prochase; i++) {
        ret[i]= pthread_create(&id2[i], NULL, prochase, NULL);
        if(ret[i] != 0) {
            printf("消费者%d线程创建失败！\n", i);
            exit(1);
        }
    }
//TODO: 等待线程销毁
    for(i = 0; i < N_product; i++) {
        pthread_join(id1[i], NULL);
        pthread_join(id2[i],NULL);
    }
    exit(0);
}