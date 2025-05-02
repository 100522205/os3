#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#include <pthread.h>

Queue* queue_init(int size) {
    Queue *q = malloc(sizeof(Queue));
    if (!q) return NULL;

    q->buffer = malloc(sizeof(struct element*) * size);
    if (!q->buffer) {
        free(q);
        return NULL;
    }

    q->max_size = size;
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    return q;
}

void queue_destroy(Queue *q) {
    if (!q) return;
    free(q->buffer);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
    free(q);
}

int queue_put(Queue *q, struct element *elem) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == q->max_size) pthread_cond_wait(&q->not_full, &q->mutex);

    q->buffer[q->tail] = elem;
    q->tail = (q->tail + 1) % q->max_size;
    q->count++;

    printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

struct element *queue_get(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) pthread_cond_wait(&q->not_empty, &q->mutex);

    struct element *elem = q->buffer[q->head];
    q->head = (q->head + 1) % q->max_size;
    q->count--;

    printf("[OK][queue] Obtained element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return elem;
}

int queue_empty(Queue *q) {
    return q->count == 0;
}

int queue_full(Queue *q) {
    return q->count == q->max_size;
}
