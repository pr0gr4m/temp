#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

struct msgbuf {
	long mtype;
	char mtext[80];
};

int main(void)
{
	struct msgbuf mesg;
	key_t key;
	int msgid, len;

	key = ftok("key", 1);
	if ((msgid = msgget(key, 0)) < 0) {
		perror("msgget");
		exit(1);
	}

	len = msgrcv(msgid, &mesg, 80, 0, 0);
	printf("Message = %s, len = %d\n", mesg.mtext, len);
	return 0;
}
