#ifndef HEADER_FILE
#define HEADER_FILE

#include <pthread.h>

struct element {
    int num_edition;
    int id_belt;
    int last;
};

typedef struct {
    struct element **buffer;
    int max_size, head, tail, count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Queue;

Queue* queue_init(int size);
void queue_destroy(Queue *q);
int queue_put(Queue *q, struct element* elem);
struct element *queue_get(Queue *q);
int queue_empty(Queue *q);
int queue_full(Queue *q);

#endif
