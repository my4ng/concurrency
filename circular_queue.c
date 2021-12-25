#include <malloc.h>
#include <string.h>
#include "circular_queue.h"

int circular_queue_init(CircularQueue *circularQueue, uint32_t elementSize, uint32_t size) {
    circularQueue->buffer = malloc(elementSize * size);
    if (circularQueue->buffer == NULL) return 1;

    *circularQueue = (CircularQueue) {.in = 0, .out = 0, .elementSize = elementSize, .size = size,
                                      .pushMutex = PTHREAD_MUTEX_INITIALIZER, .popMutex = PTHREAD_MUTEX_INITIALIZER};
    sem_init(&circularQueue->spaceSemaphore, 0, size);
    sem_init(&circularQueue->itemSemaphore, 0, 0);
    return 0;
}

void circular_queue_push(CircularQueue *circularQueue, void *data) {
    sem_wait(&circularQueue->spaceSemaphore);
    pthread_mutex_lock(&circularQueue->pushMutex);
    memcpy(circularQueue->buffer + (circularQueue->in * circularQueue->elementSize), data, circularQueue->elementSize);
    circularQueue->in = circularQueue->in + 1 < circularQueue->size ? circularQueue->in + 1 : 0;
    pthread_mutex_unlock(&circularQueue->pushMutex);
    sem_post(&circularQueue->itemSemaphore);
}

void circular_queue_pop(CircularQueue *circularQueue, void *copy) {
    sem_wait(&circularQueue->itemSemaphore);
    pthread_mutex_lock(&circularQueue->popMutex);
    memcpy(copy, circularQueue->buffer + (circularQueue->out * circularQueue->elementSize), circularQueue->elementSize);
    circularQueue->out = circularQueue->out + 1 < circularQueue->size ? circularQueue->out + 1 : 0;
    pthread_mutex_unlock(&circularQueue->popMutex);
    sem_post(&circularQueue->spaceSemaphore);
}

void circular_queue_free(CircularQueue *circularQueue) {
    free(circularQueue->buffer);
}
