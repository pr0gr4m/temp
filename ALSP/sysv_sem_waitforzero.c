#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "ipcalsp.h"

int handle_criticalsect(int, int);
int handle_waitforzero(int);

int main(int argc, char *argv[])
{
	int i, n_child, status, sem_id;

	if (argc != 2) {
		printf("%s [# of child]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	n_child = atoi(argv[1]);
	sem_id = sysv_semget(NULL, 0x12340001, 1, 2, 0660);
	
	if (fork() == 0) {
		handle_waitforzero(sem_id);
		exit(EXIT_SUCCESS);
	}

	sleep(1);

	for (i = 0; i < n_child; i++) {
		switch ( fork() ) {
			case 0:		/* child */
				handle_criticalsect(sem_id, i);
				exit(EXIT_SUCCESS);
			case -1:	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d] \n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parrent */
				break;
		}
		usleep(100000);
	}

	for (i = 0; i < n_child + 1; i++) {
		waitpid(-1, &status, 0);
	}

	if (sysv_semrm(sem_id) == -1) {
		perror("FAIL: sysv_semrm");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int handle_criticalsect(int sem_id, int idx_child)
{
	printf("[Child:%d] #1: semval(%d) semncnt(%d)\n", idx_child,
			semctl(sem_id, 0, GETVAL),
			semctl(sem_id, 0, GETNCNT));
	sysv_semwait(sem_id, 0);
	if (idx_child == 2)
		abort();
	sleep(2);
	printf("[Child:%d] #2: semval(%d) semncnt(%d)\n", idx_child,
			semctl(sem_id, 0, GETVAL),
			semctl(sem_id, 0, GETNCNT));
	sysv_sempost(sem_id, 0);
	printf("\t[Child:%d] Exiting\n\n", idx_child);
	return 0;
}

int handle_waitforzero(int sem_id)
{
	printf("<WFZ> Wait-for-zero... semval(%d)\n", semctl(sem_id, 0, GETVAL));
	sysv_semzwait(sem_id, 0);
	printf("<WFZ> Wake-up... semval(%d)\n", semctl(sem_id, 0, GETVAL));
	return 0;
}
