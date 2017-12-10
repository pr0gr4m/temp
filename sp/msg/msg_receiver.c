#include "common.h"

int main(void)
{
    key_t key;
    int msgid;
    ssize_t read;
    struct msgbuf buf;

    key = ftok(MQ_PATH, MQ_ID);
    if ((msgid = msgget(key, 0)) < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        memset(&buf, 0, sizeof(buf));

        if ((read = msgrcv(msgid, &buf, BUFSIZ, MSG_TYPE, 0)) == -1)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (!strncmp(buf.mtext, MSG_QUIT, strlen(MSG_QUIT)))
            break;

        printf("msg >> %s", buf.mtext);
    }

    return 0;
}
