#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 2
#define THREADS 1 // 1 producer and 1 consumer
#define LOOPS 3 * BUF_SIZE // 6 lần

int buffer[BUF_SIZE];
int fill = 0;
int use = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % BUF_SIZE;
}

int get() {
    int tmp = buffer[use];
    buffer[use] = -1; // clear the item
    use = (use + 1) % BUF_SIZE;
    return tmp;
}

void *producer(void *arg) {
    int i;
    int tid = (int)(long)arg;
    for (i = 0; i < LOOPS; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        put(i); // line P2
        printf("Producer %d put data %d\n", tid, i);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int tmp = 0;
    int tid = (int)(long)arg;
    while (tmp != -1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        tmp = get(); // line C2
        printf("Consumer %d get data %d\n", tid, tmp);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int i;
    int tid[THREADS];
    pthread_t producers[THREADS];
    pthread_t consumers[THREADS];

    sem_init(&empty, 0, BUF_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < THREADS; i++) {
        tid[i] = i;
        pthread_create(&producers[i], NULL, producer, (void *)(long)tid[i]);
        pthread_create(&consumers[i], NULL, consumer, (void *)(long)tid[i]);
    }

    for (i = 0; i < THREADS; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
