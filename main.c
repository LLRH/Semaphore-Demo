#include <stdio.h>
#include <pthread.h>
#define N 10
pthread_t ntid[N];
pthread_mutex_t mutex;
int counter=0;

void printids(const char *s){
    pid_t pid;
    pthread_t tid;
    pid=getpid();
    tid=pthread_self();
    printf("%s pid %u tid %u (0x%x)\n",s,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid);

}

void * thread(void *arg){
    printids("new thread:");
    pthread_mutex_lock(&mutex);
    int temp=counter;
    temp++;
    int i;
    for(i=0;i<10000000;i++){

    }
    counter=temp;
    pthread_mutex_unlock(&mutex);
    return ((void *)0);
}

int main() {
    pthread_mutex_init(&mutex,NULL);
    int temp=0;
    int i=0;
    for(i=0;i<N;i++){
        temp=pthread_create(&ntid[i],NULL,thread,NULL);
    }
    for(i=0;i<N;i++){
        pthread_join(ntid[i],NULL);
    }
    if(temp!=0){
        printf("Failure\n");
    }
    printids("main thread:");
    printf("counter=%d\n",counter);
    return 0;
}