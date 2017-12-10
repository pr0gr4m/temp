#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_inc(void *arg);
char thread1[] = "A Thread";
char thread2[] = "B Thread";

pthread_mutex_t mtx;
int number = 0;

int main(int argc, char *argv[])
{
	pthread_t t1, t2;
	int state;

	state = pthread_mutex_init(&mtx, NULL);
	if (state) {
		perror("mutex init");
		exit(EXIT_FAILURE);
	}

	pthread_create(&t1, NULL, thread_inc, (void *)thread1);
	pthread_create(&t2, NULL, thread_inc, (void *)thread2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("final number : %d \n", number);
	pthread_mutex_destroy(&mtx);
	return 0;
}

void *thread_inc(void *arg)
{
	for (int i = 0; i < 5; i++) {
		pthread_mutex_lock(&mtx);
		number++;
		printf("execute : %s, number : %d \n", (char *)arg, number);
		pthread_mutex_unlock(&mtx);
	}
}
