#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode)
{
	key_t	msg_key;
	int		msg_id;
	char	buf_err[128];

	if (tok != NULL) {
		if ((msg_key = ftok(tok, 1234)) == -1) {
			perror("ftok error");
			return -1;
		}
	} else {
		msg_key = msg_fixkey;
	}

	if ((msg_id = msgget(msg_key, IPC_CREAT | IPC_EXCL | user_mode)) == -1) {
		if (errno == EEXIST) {
			msg_id = msgget(msg_key, 0);
		}
	}

	if (msg_id == -1) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr, "FAIL: msgget(%s) [%s:%d]\n", buf_err, __FUNCTION__, __LINE__);
	}

	return msg_id;
}

int sysv_msgrm(int msg_id) {
	if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "FAIL: msgctl [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}
