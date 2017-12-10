#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define MAXLINE 511
#define MAXSOCK 1024
#define LISTEN_BACKLOG  256
static const char *exit_string = "exit";
static const char *start_string = "connected to char_server\n";

int maxfdp1;
int num_chat = 0;
int clisock_list[MAXSOCK];
int listen_sock;

void addclient(int s, struct sockaddr_storage *newcliaddr, socklen_t addrlen);
int getmax();
void removeclient(int s);
int tcp_listen(char *port, int backlog);
void errquit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    struct sockaddr_storage cliaddr;
    char buf[MAXLINE + 1];
    int i, j, nbyte, acpt_sock;
    socklen_t addrlen;
    fd_set read_fds;

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    listen_sock = tcp_listen(argv[1], LISTEN_BACKLOG);

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(listen_sock, &read_fds);
        for (i = 0; i < num_chat; i++)
            FD_SET(clisock_list[i], &read_fds);

        maxfdp1 = getmax() + 1;
        puts("wait for client");
        if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0)
            errquit("select fail");

        if (FD_ISSET(listen_sock, &read_fds))
        {
            addrlen = sizeof(cliaddr);
            acpt_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
            if (acpt_sock == -1)
                errquit("accept fail");
            addclient(acpt_sock, &cliaddr, addrlen);
            send(acpt_sock, start_string, strlen(start_string), 0);
            printf("%d th user addition \n", num_chat);
        }

        for (i = 0; i < num_chat; i++)
        {
            if (FD_ISSET(clisock_list[i], &read_fds))
            {
                nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
                if (nbyte == 0)
                {
                    removeclient(i);
                    continue;
                }

                buf[nbyte] = 0;
                if (strstr(buf, exit_string) != NULL)
                {
                    removeclient(i);
                    continue;
                }

                for (j = 0; j < num_chat; j++)
                    send(clisock_list[j], buf, nbyte, 0);
                printf("%s \n", buf);
            }
        }
    }

    return 0;
}

void addclient(int s, struct sockaddr_storage *newcliaddr, socklen_t addrlen)
{
    char addrstr[INET6_ADDRSTRLEN], portstr[8];
    int rc_gai = getnameinfo((struct sockaddr *)newcliaddr, addrlen, addrstr, sizeof(addrstr),
            portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);
    if (rc_gai)
    {
        fprintf(stderr, "Fail: getnameinfo(): %s\n", gai_strerror(rc_gai));
        exit(EXIT_FAILURE);
    }

    if (newcliaddr->ss_family == AF_INET)
    {
        printf("new client (ip:port) (%s:%s) \n", addrstr, portstr);
    }
    else if (newcliaddr->ss_family == AF_INET6)
    {
        printf("new client (ip:port,scope) (%s:%s,%d) \n", addrstr, portstr,
                ((struct sockaddr_in6 *)newcliaddr)->sin6_scope_id);
    }
    clisock_list[num_chat++] = s;
}

void removeclient(int s)
{
    close(clisock_list[s]);
    if (s != num_chat - 1)
        clisock_list[s] = clisock_list[num_chat - 1];
    printf("Logouted one of the chatting user, number of chatting user : %d\n",
            --num_chat);
}

int getmax()
{
    int max = listen_sock;
    for (int i = 0; i < num_chat; i++)
        if (clisock_list[i] > max)
            max = clisock_list[i];
    return max;
}

int tcp_listen(char *port, int backlog)
{
    int sd;
    struct addrinfo ai, *ai_ret;
    int rc_gai;

    memset(&ai, 0, sizeof(ai));
    ai.ai_family = AF_INET;
    ai.ai_socktype = SOCK_STREAM;
    ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

    if ((rc_gai = getaddrinfo(NULL, port, &ai, &ai_ret)) != 0)
    {
        fprintf(stderr, "Fail: getaddrinfo(): %s \n", gai_strerror(rc_gai));
        exit(EXIT_FAILURE);
    }

    if ((sd = socket(ai_ret->ai_family,
                    ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1)
        errquit("socket fail");

    if (bind(sd, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1)
        errquit("bind fail");

    listen(sd, backlog);
    return sd;
}
