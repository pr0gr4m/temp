#include "shm_sem.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	int shmid, read_semid, write_semid;
	int *shmptr;

	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short int *array;
	} arg;

	shmid = shmget(SHM_KEY, SHM_SIZE, 0600 | IPC_CREAT);
	shmptr = (int *)shmat(shmid, 0, 0);
	
	read_semid = semget(READ_KEY, 1, 0600 | IPC_CREAT);
	arg.val = 0;
	semctl(read_semid, 0, SETVAL, arg);

	write_semid = semget(WRITE_KEY, 1, 0600 | IPC_CREAT);
	arg.val = 1;
	semctl(write_semid, 0, SETVAL, arg);
	
	for (int i = 0; i < 10; i++) {
		p(read_semid);
		printf("read : %d \n", *shmptr);
		v(write_semid);
		sleep(1);
	}

	shmctl(shmid, IPC_RMID, 0);
	semctl(read_semid, 0, IPC_RMID, 0);
	semctl(write_semid, 0, IPC_RMID, 0);

	return 0;
}
