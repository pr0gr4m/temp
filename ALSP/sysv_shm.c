#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SZ_SHM_SEGMENT	4096

int sysv_shmget(void **ret, char *tok, key_t shm_fixkey, int size, int user_mode);
int sysv_shmrm(int shm_id);

int main()
{
	int shm_id;
	char *shm_ptr;
	int n_read = 0;
	size_t n_input = 128;
	char *p_input = (char *)malloc(n_input);

	printf("c : Create shared memory without key.\n");
	printf("number : attach shared memory with IPC id number.\n>> ");

	if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) {
		perror("getline error");
		return -1;
	}
	if (p_input[0] == 'c') {
		shm_id = sysv_shmget((void **)&shm_ptr, NULL, IPC_PRIVATE, SZ_SHM_SEGMENT, 0664);
		if (shm_id == -1) {
			perror("sysv_shmget error");
			exit(EXIT_FAILURE);
		}
	} else {
		shm_id = atoi(p_input);
		if ((shm_ptr = (char *)shmat(shm_id, 0, 0)) == (char *)-1) {
			perror("shmat error");
			exit(EXIT_FAILURE);
		}
	}
	printf("* SHM IPC id(%d), PID(%d)\n", shm_id, getpid());
	printf("'*' Print current shm.\n'.' Exit. Otherwise input text to shm.\n");
	printf("'?' print shm info\n");

	while (1) {
		printf("\n>> ");
		if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) {
			perror("getline error");
			return -1;
		}
		if (p_input[0] == '.') {
			break;
		} else if (p_input[0] == '?') {
			struct shmid_ds shm_ds;
			if (shmctl(shm_id, IPC_STAT, &shm_ds) == -1) {
				perror("shmctl error");
				continue;
			}
			printf("size(%ld) # of attach(%ld)\n", shm_ds.shm_segsz, shm_ds.shm_nattch);
			printf("shm_cpid(%d) shm_lpid(%d)\n", shm_ds.shm_cpid, shm_ds.shm_lpid);
		} else if (p_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			p_input[n_read - 1] = '\0';
			memcpy(shm_ptr, p_input, n_read - 1);
		}
	}

	printf("* Would you remove shm (IPD id : %d) (y/n) ", shm_id);
	if ( (n_read = (int)getline(&p_input, &n_input, stdin)) == -1) {
		perror("getline error -> shm remove fail");
		return -1;
	}
	if (p_input[0] == 'y') {
		sysv_shmrm(shm_id);
	}

	return 0;
}
