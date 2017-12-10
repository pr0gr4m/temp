#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <limits.h>

int main(int argc, char *argv[])
{
	char posix_sem_name[NAME_MAX];

	if (argc != 2) {
		printf("%s [SEM_NAME]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(posix_sem_name, argv[1], NAME_MAX);
	
	sem_t *p_psem = sem_open(posix_sem_name, 0);
	if (p_psem == SEM_FAILED) {
		perror("sem not exist");
		exit(EXIT_FAILURE);
	} else
		printf("[%d] Attach to an existed sem\n", getpid());

	int sem_value;
	sem_post(p_psem);
	printf("[%d] Atomically increase sem -> value(%d)\n", getpid(), sem_getvalue(p_psem, &sem_value));
	sem_close(p_psem);
	return 0;
}
