#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int MAX_COUNT = 1e9;
static int count = 0;
pthread_mutex_t lock;

void* fcount(void* sid) {
    pthread_mutex_lock(&lock);  // Lock toàn bộ khối for
    for (int i = 0; i < MAX_COUNT; i++) {
//	pthread_mutex_lock(&lock);
        count = count + 1;
//	pthread_mutex_unlock(&lock);
    }
    pthread_mutex_unlock(&lock);  // Unlock sau khi xong
    printf("Thread %s: holding %d\n", (char*) sid, count);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&thread1, NULL, fcount, "1");
    pthread_create(&thread2, NULL, fcount, "2");

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}
