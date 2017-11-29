#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

//TODO ��windowsϵͳ��������� sleep(2) ->> Sleep(2000)
//#include <windows.h>

#define N_product 5   // �����ߵ���Ŀ
#define N_prochase 5   // �����ߵ���Ŀ
#define sleep_product 2 //������˯��ʱ��
#define sleep_prochase 5 //������˯��ʱ��
#define M 50 // ������Ŀ
int in = 0; // �����߷��ò�Ʒ��λ��
int out = 0; // ������ȡ��Ʒ��λ��

int buff[M] = { 0 }; // �����ʼ��Ϊ0����ʼʱû�в�Ʒ


sem_t empty_sem; // ͬ���ź�����������ʱ��ֹ�����߷Ų�Ʒ
sem_t full_sem; // ͬ���ź�������û��Ʒʱ��ֹ����������

pthread_mutex_t mutex_product; // �����ź�����һ��ֻ��һ���̷߳��ʻ���
pthread_mutex_t mutex_prochase; // �����ź�����һ��ֻ��һ���̷߳��ʻ���

int product_id = 0; //������id
int prochase_id = 0; //������id
//�źŴ�����
void Handlesignal(int signo){
    printf("�����˳�\n",signo);
    exit(0);
}
/* ��ӡ������� */
void print() {
    int i;
    printf("��Ʒ����Ϊ");
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

/* �����߷��� */
void *product() {
    int id = ++product_id;
    while(1) {//�ظ�����
        //��sleep���������Ե������������ѵ��ٶȣ����ڹ۲�
        sleep(sleep_product);

        sem_wait(&empty_sem);
        pthread_mutex_lock(&mutex_product);

        in= in % M;
        printf(" + ������%d�ڲ�Ʒ�����з����%d����Ʒ\t",id, in);
        fflush(stdout);

        buff[in]= 1;
        print();
        ++in;

        pthread_mutex_unlock(&mutex_product);
        sem_post(&full_sem);
    }
}

/* �����߷��� */
void *prochase() {
    int id = ++prochase_id;
    while(1) {//�ظ�����
        //��sleep���������Ե������������ѵ��ٶȣ����ڹ۲�
        sleep(sleep_prochase);

        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex_prochase);

        out= out % M;
        printf(" - ������%d�Ӳ�Ʒ������ȡ����%d����Ʒ\t",id, out);
        fflush(stdout);

        buff[out]= 0;
        print();
        ++out;

        pthread_mutex_unlock(&mutex_prochase);
        sem_post(&empty_sem);
    }
}

int main() {
    printf("������%d����������%d��,"
            "��Ʒ����Ϊ%d,������ÿ%d������һ����Ʒ��"
            "������ÿ%d������һ����Ʒ,Ctrl+�˳�����\n",
             N_product,N_prochase,M,sleep_product,sleep_prochase);
    pthread_t id1[N_prochase];
    pthread_t id2[N_product];
    int i;
    int ret[N_prochase+N_product];
    //��������
    if(signal(SIGINT,Handlesignal)==SIG_ERR){//��ctrl+C����SIGINT�ź�
        printf("�źŰ�װ����\n");
    }
//TODO: ��ʼ��ͬ���ź���
    int ini1 = sem_init(&empty_sem, 0, M);//��Ʒ���л���ͬ��
    int ini2 = sem_init(&full_sem, 0, 0);//�߳�����ͬ��
    if(ini1 && ini2 != 0) {
        printf("�ź�����ʼ��ʧ�ܣ�\n");
        exit(1);
    }
//TODO: ��ʼ�������ź���
    int ini3 = pthread_mutex_init(&mutex_product, NULL);
    int ini4 = pthread_mutex_init(&mutex_prochase, NULL);
    if(ini3 != 0 ||ini4!=0) {
        printf("�߳�ͬ����ʼ��ʧ�ܣ�\n");
        exit(1);
    }
//TODO: ����N���������߳�
    for(i = 0; i < N_product; i++) {
        ret[i]= pthread_create(&id1[i], NULL, product, (void *) (&i));
        if(ret[i] != 0) {
            printf("������%d�̴߳���ʧ�ܣ�\n", i);
            exit(1);
        }
    }
//TODO: ����N���������߳�
    for(i = 0; i < N_prochase; i++) {
        ret[i]= pthread_create(&id2[i], NULL, prochase, NULL);
        if(ret[i] != 0) {
            printf("������%d�̴߳���ʧ�ܣ�\n", i);
            exit(1);
        }
    }
//TODO: �ȴ��߳�����
    for(i = 0; i < N_product; i++) {
        pthread_join(id1[i], NULL);
        pthread_join(id2[i],NULL);
    }
    exit(0);
}