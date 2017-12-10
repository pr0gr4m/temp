#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MQ_PATH     "/tmp/msgq"
#define MQ_ID       1
#define MSG_TYPE    1
#define MSG_QUIT    "quit"

struct msgbuf
{
    long mtype;
    char mtext[BUFSIZ];
};

#endif
