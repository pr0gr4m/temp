#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	key_t key;
	int msgid;

	key = ftok("key", 1);
	msgid = msgget(key, 0);

	if (msgid == -1) {
		perror("msgget");
		exit(1);
	}

	printf("Before \n");
	system("ipcs -q");
	msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);
	printf("After \n");
	system("ipcs -q");

	return 0;
}
