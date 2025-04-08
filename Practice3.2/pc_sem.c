#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUF_SIZE 2
#define THREADS 1
#define LOOPS (3 * BUF_SIZE)

int buffer[BUF_SIZE];
int fill = 0;
int use = 0;

sem_t empty, full;
pthread_mutex_t mutex;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % BUF_SIZE;
}

int get() {
    int tmp = buffer[use];
    buffer[use] = -1;  // clear value (not necessary, but good for debugging)
    use = (use + 1) % BUF_SIZE;
    return tmp;
}

void* producer(void* arg) {
    int i;
    int tid = *(int*)arg;
    for (i = 0; i < LOOPS; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        put(i);
        printf("Producer %d put data %d\n", tid, i);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    int tmp = 0;
    int tid = *(int*)arg;
    int i;
    for (i = 0; i < LOOPS; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        tmp = get();
        printf("Consumer %d get data %d\n", tid, tmp);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int i;
    pthread_t producers[THREADS];
    pthread_t consumers[THREADS];
    int tid[THREADS];

    sem_init(&empty, 0, BUF_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < THREADS; i++) {
        tid[i] = i;
        pthread_create(&producers[i], NULL, producer, &tid[i]);
        pthread_create(&consumers[i], NULL, consumer, &tid[i]);
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
