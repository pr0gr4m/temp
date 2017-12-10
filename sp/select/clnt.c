#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXLINE 511
#define NAME_LEN    20

static const char *exit_string = "exit";

int tcp_connect(int af, char *servip, char *port);
void errquit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char bufall[MAXLINE + NAME_LEN + 1];
    char *bufmsg;
    int maxfdp1;
    int s;
    int namelen;
    fd_set read_fds;

    if (argc != 4)
    {
        printf("Usage : %s <server_ip> <port> <name> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sprintf(bufall, "[%s] : ", argv[3]);
    namelen = strlen(bufall);
    bufmsg = bufall + namelen;

    if ((s = tcp_connect(AF_INET, argv[1], argv[2])) == -1)
        errquit("tcp connect fail");

    puts("connect to server");
    maxfdp1 = s + 1;
    FD_ZERO(&read_fds);

    while (1)
    {
        FD_SET(0, &read_fds);
        FD_SET(s, &read_fds);
        
        if (select(maxfdp1, &read_fds, NULL, NULL, NULL) == -1)
            errquit("select fail");

        if (FD_ISSET(s, &read_fds))
        {
            int nbyte;
            if ((nbyte = recv(s, bufmsg, MAXLINE, 0)) > 0)
            {
                bufmsg[nbyte] = 0;
                printf("%s \n", bufmsg);
            }
        }

        if (FD_ISSET(0, &read_fds))
        {
            if (fgets(bufmsg, MAXLINE, stdin))
            {
                if (send(s, bufall, namelen + strlen(bufmsg), 0) < 0)
                    puts("Error: write error on select");
                
                if (strstr(bufmsg, exit_string) != NULL)
                {
                    puts("Good bye!");
                    close(s);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }

    return 0;
}

int tcp_connect(int af, char *servip, char *port)
{
    int s, rc_gai;
    struct addrinfo ai, *ai_ret;

    memset(&ai, 0, sizeof(ai));
    ai.ai_family = af;
    ai.ai_socktype = SOCK_STREAM;
    ai.ai_flags = AI_ADDRCONFIG;

    if ((rc_gai = getaddrinfo(servip, port, &ai, &ai_ret)) != 0)
    {
        fprintf(stderr, "Fail: getaddrinfo(): %s \n", gai_strerror(rc_gai));
        exit(EXIT_FAILURE);
    }

    if ((s = socket(ai_ret->ai_family, ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1)
        errquit("socket fail");

    if (connect(s, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1)
        errquit("connect fail");

    return s;
}
