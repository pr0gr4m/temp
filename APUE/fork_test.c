#include "apue.h"
#include <pthread.h>

void *test_func(void *arg)
{
	puts("I'm new thread");
}

int main(void)
{
	int err;
	pid_t pid;
	pthread_t id;

	pid = fork();

	if (pid == 0)
		fork();
	else if (pid > 0)
	{
		fork();
		err = pthread_create(&id, NULL, test_func, NULL);
		if (err != 0)
			err_exit(err, "can't create thread");
	}
	fork();

	sleep(3);

	exit(0);
}
