/*
 *
 * process_manager.c
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <stddef.h>
 #include <pthread.h>
 #include "queue.h"
 #include <semaphore.h>
 
 void *producer(void *arg);
 void *consumer(void *arg);
 
 struct thread_args {
	 int id, total;
	 Queue *q;
 };
 
 void *producer(void *arg) {
	 struct thread_args *args = (struct thread_args *)arg;
	 for (int i = 0; i < args->total; i++) {
		 struct element *e = malloc(sizeof(struct element));
		 if (!e) continue;
		 e->num_edition = i;
		 e->id_belt = args->id;
		 e->last = (i == args->total - 1);
		 queue_put(args->q, e);
	 }
	 pthread_exit(NULL);
 }
 
 void *consumer(void *arg) {
	 struct thread_args *args = (struct thread_args *)arg;
	 int done = 0;
	 while (!done) {
		 struct element *e = queue_get(args->q);
		 done = (e->last == 1);
		 free(e);
	 }
	 pthread_exit(NULL);
 }
 
 int process_manager(int id, int belt_size, int items_to_produce) {
	 if (id < 0 || belt_size <= 0 || items_to_produce <= 0) {
		 fprintf(stderr, "[ERROR][process_manager] Arguments not valid.\n");
		 return -1;
	 }
 
	 printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n", id, items_to_produce);
 
	 Queue *q = queue_init(belt_size);
	 if (!q) {
		 fprintf(stderr, "[ERROR][process_manager] Failed to initialize belt for id %d.\n", id);
		 return -1;
	 }
	 printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", id, belt_size);
 
	 pthread_t prod, cons;
	 struct thread_args args = {id, items_to_produce, q};
 
	 pthread_create(&prod, NULL, producer, &args);
	 pthread_create(&cons, NULL, consumer, &args);
 
	 pthread_join(prod, NULL);
	 pthread_join(cons, NULL);
 
	 printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", id, items_to_produce);
	 queue_destroy(q);
	 return 0;
 }
 