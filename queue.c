#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#include <pthread.h>

static struct element **buffer = NULL;
static int max_size = 0, head = 0, tail = 0, count = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

int queue_init(int size) {
    buffer = (struct element **) malloc(sizeof(struct element *) * size);
    if (!buffer) return -1;
    max_size = size;
    head = tail = count = 0;
    return 0;
}

int queue_destroy(void) {
    free(buffer);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    return 0;
}

int queue_put(struct element *elem) {
    pthread_mutex_lock(&mutex);
    while (count == max_size) pthread_cond_wait(&not_full, &mutex);

    buffer[tail] = elem;
    tail = (tail + 1) % max_size;
    count++;

    printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
    return 0;
}

struct element *queue_get(void) {
    pthread_mutex_lock(&mutex);
    while (count == 0) pthread_cond_wait(&not_empty, &mutex);

    struct element *elem = buffer[head];
    head = (head + 1) % max_size;
    count--;

    printf("[OK][queue] Obtained element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return elem;
}

int queue_empty(void) {
    return count == 0;
}

int queue_full(void) {
    return count == max_size;
}