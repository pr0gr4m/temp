#include "common.h"

int main(void)
{
    key_t key;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int msgid;
    struct msgbuf buf;

    key = ftok(MQ_PATH, MQ_ID);
    if ((msgid = msgget(key, IPC_CREAT | 0644)) == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    
    while (1)
    {
        printf(">> ");
        if ((read = getline(&line, &len, stdin)) == -1)
        {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        memset(&buf, 0, sizeof(buf));
        buf.mtype = MSG_TYPE;
        strncpy(buf.mtext, line, read);

        if (msgsnd(msgid, (void *)&buf, BUFSIZ, IPC_NOWAIT) == -1)
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        if (!strncmp(MSG_QUIT, line, strlen(MSG_QUIT)))
            break;
    }

    free(line);

    return 0;
}
