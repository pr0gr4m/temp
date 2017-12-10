#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
	int shmid;
	char *memptr;
	key_t key;
	
	key = ftok("key", 1);

	if ((shmid = shmget(key, 64, 0600 | IPC_CREAT | IPC_EXCL)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((memptr = shmat(shmid, (char *)0, 0)) == (char *)-1) {
		perror("shmat");
		exit(1);
	}

	strcpy(memptr, "Hello World!");
	printf("shmkey = %d, shmid = %d \n", key, shmid);
	printf("memptr = %p : %s\n", memptr, memptr);

	sleep(1);

	printf("\nAfter shmget...\n");

	system("ipcs -m");

	if (shmdt(memptr) < 0) {
		perror("shmdt");
		exit(1);
	}

	if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) < 0) {
		perror("shmctl");
		exit(1);
	}

	printf("\nAfter remove...\n");
	system("ipcs -m");

	return 0;
}
	
