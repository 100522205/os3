/*
 *
 * factory_manager.c
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <stddef.h>
 #include <semaphore.h>
 #include <sys/stat.h>
 #include <pthread.h>
 
 extern int process_manager(int id, int belt_size, int items);
 
 struct args {
	 int id, belt_size, items;
 };
 
 void *thread_fn(void *arg) {
	 struct args *a = (struct args *)arg;
	 int ret = process_manager(a->id, a->belt_size, a->items);
	 if (ret != 0)
		 fprintf(stderr, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", a->id);
	 else
		 printf("[OK][factory_manager] Process_manager with id %d has finished.\n", a->id);
	 free(a);
	 pthread_exit(NULL);
 }
 
 int main(int argc, const char *argv[]) {
	 if (argc != 2) {
		 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		 return -1;
	 }
 
	 FILE *f = fopen(argv[1], "r");
	 if (!f) {
		 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		 return -1;
	 }
 
	 int max, id, size, items, count = 0;
	 pthread_t threads[64];
 
	 if (fscanf(f, "%d", &max) != 1 || max <= 0) {
		 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		 fclose(f);
		 return -1;
	 }
 
	 while (fscanf(f, "%d %d %d", &id, &size, &items) == 3) {
		 if (count >= max) {
			 fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
			 fclose(f);
			 return -1;
		 }
		 struct args *a = malloc(sizeof(struct args));
		 a->id = id;
		 a->belt_size = size;
		 a->items = items;
		 pthread_create(&threads[count], NULL, thread_fn, a);
		 printf("[OK][factory_manager] Process_manager with id %d has been created.\n", id);
		 count++;
	 }
 
	 fclose(f);
	 for (int i = 0; i < count; i++) pthread_join(threads[i], NULL);
	 printf("[OK][factory_manager] Finishing.\n");
	 return 0;
 }