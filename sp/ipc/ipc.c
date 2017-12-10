#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define PATH_REQ    "/tmp/fifo_req"
#define PATH_RES    "/tmp/fifo_res"

#define CORRECT     1
#define WRONG       0

int procparent();
int procchild();
int check(char buf[]);

int main()
{
    pid_t pid;

    if (mkfifo(PATH_REQ, 0600) == -1 && errno != EEXIST)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(PATH_RES, 0600) == -1 && errno != EEXIST)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    switch (pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            return procchild();

        default:
            return procparent();
    }
    
    return 0;   // never reached
}

int procparent()
{
    int fd_req, fd_res;
    char *line = NULL;
    size_t len = 0;
    ssize_t ret;
    int res;

    printf("I am parent. My pid is [%ld]\n", (long) getpid());

    while (((fd_req = open(PATH_REQ, O_WRONLY)) == -1) && (errno == EINTR));
    while (((fd_res = open(PATH_RES, O_RDONLY)) == -1) && (errno == EINTR));

    if (fd_req == -1 || fd_res == -1)
    {
        perror("open fifo");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (getline(&line, &len, stdin) == -1)
        {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        if ((ret = write(fd_req, line, 3)) != 3)
        {
            perror("write");
            continue;
        }

        if (read(fd_res, &res, sizeof(int)) == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (res == CORRECT)
            break;
    }

    free(line);
    return 0;
}

int procchild()
{
    int fd_req, fd_res;
    ssize_t ret;
    char buf[BUFSIZ];
    int res;

    char num[] = "385";

    printf("I am child. My pid is [%ld]\n", (long) getpid());

    while (((fd_req = open(PATH_REQ, O_RDONLY)) == -1) && (errno == EINTR));
    while (((fd_res = open(PATH_RES, O_WRONLY)) == -1) && (errno == EINTR));
    if (fd_req == -1 || fd_res == -1)
    {
        perror("open fifo");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        memset(buf, 0, sizeof(buf));

        if ((ret = read(fd_req, buf, BUFSIZ)) != 3)
        {
            perror("read");
            continue;
        }

        if (check(buf) == CORRECT)
        {
            res = CORRECT;
            if (write(fd_res, &res, sizeof(int)) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
            break;
        }
        else
        {
            res = WRONG;
            if (write(fd_res, &res, sizeof(int)) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}

int check(char buf[])
{
    int strike = 0;
    int ball = 0;
    char goal[] = "385";    // it can make with rand

    if (!strncmp(buf, goal, 3))
    {
        printf("[%ld] : %s - 3 Strike! \n", (long)getpid(), buf);
        return CORRECT;
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (goal[i] == buf[j])
            {
                if (i == j)
                    strike++;
                else
                    ball++;
            }
        }
    }

    printf("[%ld] : %s - %d Strike, %d Ball !\n", (long)getpid(), buf, strike, ball);

    return WRONG;
}
