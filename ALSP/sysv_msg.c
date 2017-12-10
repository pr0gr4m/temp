#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipcalsp.h"

#define LEN_MQ_MTEXT	512

int msg_id;

struct mq_buf {
	long mtype;
	char mtext[LEN_MQ_MTEXT];
};

int start_msq_sender(char *srcfile);
int start_msq_receiver(long mtype);

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage : %s <sender | receiver> <filename or mtype> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((msg_id = sysv_msgget(argv[0], 0, 0664)) == -1) {
		exit(EXIT_FAILURE);
	}

	printf("* Message queue test program (ID:%d) \n", msg_id);

	switch (argv[1][0]) {
		case 's':	/* send */
			printf("+ Sender start with file (%s) \n", argv[2]);
			(void) start_msq_sender(argv[2]);
			printf("+ Finished. (Ctrl-C:Exit) (Press any key:Remove MQ)\n");
			getchar();
			break;
		case 'r':	/* receive */
			printf("+ Receiver start with type (%s) \n", argv[2]);
			(void) start_msq_receiver(atol(argv[2]));
			break;
		default:
			fprintf(stderr, "* Unknown option, use sender or receiver \n");
			return 1;
	}

	sysv_msgrm(msg_id);
	return 0;
}

int start_msq_sender(char *srcfile)
{
	FILE	*fp_srcfile;
	char	rbuf[LEN_MQ_MTEXT];
	struct mq_buf	mq_buf;
	int		len_mtext;

	if ((fp_srcfile = fopen(srcfile, "r")) == NULL) {
		perror("FAIL: fopen()");
		return -1;
	}

	while (!feof(fp_srcfile)) {
		if (fgets(rbuf, sizeof(rbuf), fp_srcfile) == NULL)
			break;
		mq_buf.mtype = (long) (rbuf[0] - '0');
		len_mtext = strnlen(rbuf, LEN_MQ_MTEXT) - 3;
		memcpy(mq_buf.mtext, (char *)&rbuf[2], len_mtext);
		printf("\t- Send (mtype:%ld,len:%d)-(mtext:%.*s)\n",
				mq_buf.mtype, len_mtext, len_mtext, mq_buf.mtext);
		if (msgsnd(msg_id, (void *)&mq_buf, len_mtext, IPC_NOWAIT) == -1) {
			perror("FAIL: msgsend()");
			break;
		}
	}

	fclose(fp_srcfile);
	return 0;
}

int start_msq_receiver(long mtype) {
	int		n_recv;
	struct mq_buf	mq_buf;

	while (1) {
		if ((n_recv = msgrcv(msg_id, (void *)&mq_buf, LEN_MQ_MTEXT, mtype, 0)) == -1)
			break;

		printf("+ Recv (mtype:%ld,len:%d)-(%.*s)\n",
				mq_buf.mtype, n_recv, n_recv, mq_buf.mtext);
	}

	return 0;
}
