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
	 int id, belt_size, total;
 };
 
 void *producer(void *arg) {
	 struct thread_args *args = (struct thread_args *)arg;
	 for (int i = 0; i < args->total; i++) {
		 struct element *e = malloc(sizeof(struct element));
		 if (!e) continue;
		 e->num_edition = i;
		 e->id_belt = args->id;
		 e->last = (i == args->total - 1);
		 queue_put(e);
	 }
	 pthread_exit(NULL);
 }
 
 void *consumer(void *arg) {
	 int done = 0;
	 while (!done) {
		 struct element *e = queue_get();
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
 
	 if (queue_init(belt_size) != 0) {
		 fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
		 return -1;
	 }
	 printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", id, belt_size);
 
	 pthread_t prod, cons;
	 struct thread_args args = {id, belt_size, items_to_produce};
	 pthread_create(&prod, NULL, producer, &args);
	 pthread_create(&cons, NULL, consumer, &args);
 
	 pthread_join(prod, NULL);
	 pthread_join(cons, NULL);
 
	 printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", id, items_to_produce);
	 queue_destroy();
	 return 0;
 }