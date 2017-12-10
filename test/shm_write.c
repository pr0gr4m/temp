#include "shm_sem.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	int i;
	int shmid, read_semid, write_semid;
	int *shmptr;

	shmid = shmget(SHM_KEY, SHM_SIZE, 0);
	shmptr = (int *)shmat(shmid, 0, 0);

	read_semid = semget(READ_KEY, 1, 0);
	write_semid = semget(WRITE_KEY, 1, 0);

	for (i = 0; i < 10; i++) {
		p(write_semid);
		*shmptr = i;
		printf("write : %d \n", i);
		v(read_semid);
		sleep(1);
	}

	return 0;
}
