#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int sysv_shmget(void **ret, char *tok, key_t shm_fixkey, int size, int user_mode)
{
	key_t shm_key;
	int shm_id;
	char buf_err[128];

	if (ret == NULL || size < 0)
		return -1;
	if (tok != NULL) {
		if ((shm_key = ftok(tok, 1234)) == -1) {
			return -1;
		}
	} else {
		shm_key = shm_fixkey;
	}
	if ((shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | user_mode)) == -1) {
		if (errno == EEXIST) {
			shm_id = shmget(shm_key, 0, 0);
		}
	}
	if (shm_id == -1) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr, "FAIL: shmget():%s [%d]\n", buf_err, __LINE__);
		return -1;
	}
	if ((*ret = shmat(shm_id, 0, 0)) == NULL) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr, "FAIL: shmat():%s [%d]\n", buf_err, __LINE__);
		return -1;
	}
	return shm_id;
}

int sysv_shmrm(int shm_id)
{
	int ret;
	if ((ret = shmctl(shm_id, IPC_RMID, NULL)) == -1) {
		fprintf(stderr, "FAIL:shmctl():%s [%s:%d]\n", strerror(errno), __FUNCTION__, __LINE__);
		return -1;
	}
	return ret;
}
