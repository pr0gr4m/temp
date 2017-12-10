#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>

#define NAME_POSIX_SEM	"/my_psem"

const int	max_child = 30;
sem_t	*p_psem;

int process_child(int);

int main(int argc, char *argv[])
{
	int i, n_count, sem_value, status;

	if (argc != 2) {
		printf("%s [counter]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	n_count = atoi(argv[1]);
	p_psem = sem_open(NAME_POSIX_SEM, O_CREAT | O_EXCL, 0600, n_count);

	if (p_psem == SEM_FAILED) {
		if (errno != EEXIST) {
			perror("FAIL: sem_open");
			exit(EXIT_FAILURE);
		}
		p_psem = sem_open(NAME_POSIX_SEM, 0);
		printf("[%d] Attach to an existed sem \n", getpid());
	} else
		printf("[%d] Create new sem \n", getpid());

	sem_getvalue(p_psem, &sem_value);
	printf("[%d] sem_getvalue = %d\n", getpid(), sem_value);

	for (i = 0; i < max_child; i++) {
		printf("[%d] iteration(%d) : Atomically decrease\n", getpid(), i);
		sem_wait(p_psem);

		switch ( fork() ) {
			case 0:		/* child */
				process_child(i);
				sem_post(p_psem);
				exit(EXIT_SUCCESS);
			case -1:	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d]\n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parrent */
				break;
		}
		usleep(10000);
	}

	for (i = 0; i < max_child; i++) {
		pid_t pid_child;
		if ((pid_child = waitpid(-1, &status, 0)) == -1) {
			perror("waitpid error");
			continue;
		}
	}

	sem_getvalue(p_psem, &sem_value);
	printf("[%d] sem_getvalue = %d \n", getpid(), sem_value);
	if (sem_unlink(NAME_POSIX_SEM) == -1) {
		perror("sem_unlink error");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

int process_child(int i)
{
	printf("\t[Child:%d] sleep(2)\n", i);
	sleep(2);
	return 0;
}
