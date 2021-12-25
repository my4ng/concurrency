#ifndef CONCURRENCY_CIRCULAR_QUEUE_H
#define CONCURRENCY_CIRCULAR_QUEUE_H

#include <semaphore.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct CircularQueue_t {
    sem_t spaceSemaphore;
    sem_t itemSemaphore;
    pthread_mutex_t pushMutex;
    pthread_mutex_t popMutex;

    uint32_t elementSize;
    uint32_t in;
    uint32_t out;
    uint32_t size;
    char* buffer;
} CircularQueue;

int circular_queue_init(CircularQueue *circularQueue, uint32_t elementSize, uint32_t size);
void circular_queue_push(CircularQueue *circularQueue, void *data);
void circular_queue_pop(CircularQueue *circularQueue, void *copy);
void circular_queue_free(CircularQueue *circularQueue);

#endif //CONCURRENCY_CIRCULAR_QUEUE_H
