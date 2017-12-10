#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "ipcalsp.h"

int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode)
{
	int sem_id, i;
	union semun semun;
	unsigned short int *arr_semval = NULL;
	key_t sem_key;

	if (tok != NULL) {
		if ((sem_key = ftok(tok, 1234)) == -1) {
			perror("ftok error");
			return -1;
		}
	} else {
		sem_key = sem_fixkey;
	}

	if ((sem_id = semget(sem_key, n_sem, IPC_CREAT | IPC_EXCL | user_mode)) == -1) {
		if (errno == EEXIST) {
			sem_id = semget(sem_key, n_sem, 0);
			return sem_id;
		}
	}

	if (sem_id == -1) {
		fprintf(stderr, "FAIL: semget [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if ((arr_semval = (unsigned short int *)malloc(sizeof(short int) * n_sem)) == NULL) {
		fprintf(stderr, "FAIL: malloc [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	for (i = 0; i < n_sem; i++)
		arr_semval[i] = sem_value;
	semun.array = arr_semval;
	if (semctl(sem_id, 0, SETALL, semun) == -1) {
		fprintf(stderr, "FAIL: semctl [%s:%d]\n", __FUNCTION__, __LINE__);
		free(arr_semval);
		return -1;
	}
	free(arr_semval);
	return sem_id;
}

int sysv_semrm(int sem_id)
{
	if (semctl(sem_id, 0, IPC_RMID) == -1) {
		fprintf(stderr, "FAIL: semctl() 'IPC_RMID' [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

int sysv_semval(int sem_id, int sem_idx)
{
	int semval;
	if ((semval = semctl(sem_id, sem_idx, GETVAL)) == -1) {
		fprintf(stderr, "FAIL: semctl() 'GETVAL' [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return semval;
}

int sysv_semwait(int sem_id, int sem_idx)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = 0;
	sem_buf.sem_op = -1;

	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'P' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int sysv_sempost(int sem_id, int sem_idx)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = 0;
	sem_buf.sem_op = 1;

	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'V' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int sysv_semzwait(int sem_id, int sem_idx)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = 0;
	sem_buf.sem_op = 0;

	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'WFZ' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}
