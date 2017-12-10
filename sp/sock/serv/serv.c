#include "common.h"

#define LISTEN_BACKLOG  512
#define MAX_POOL        5

void start_child(int cfd, int idx);
int recv_file(int cfd, int idx);

int main(int argc, char *argv[])
{
    int i;
    char *port;
    socklen_t len_saddr;
    pid_t pid;

    if (argc > 2) {
        printf("%s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        port = strdup(argv[1]);
    else
        port = strdup("0");

    struct addrinfo hints, *result, *rp;
    int rc_gai;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

    if ((rc_gai = getaddrinfo(NULL, port, &hints, &result)) != 0) {
        fprintf(stderr, "Fail: getaddrinfo():%s\n", gai_strerror(rc_gai));
        exit(EXIT_FAILURE);
    }

    int fd_listener;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((fd_listener = socket(rp->ai_family,
                        rp->ai_socktype, rp->ai_protocol)) == -1) {
            fprintf(stderr, "[TCP server] Fail: socket()\n");
            continue;
        }

        if (bind(fd_listener, rp->ai_addr, rp->ai_addrlen) == 0)
            break;  // success

        close(fd_listener);
    }

    if (rp == NULL) {   // No address succeeded
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    if (!strncmp(port, "0", strlen(port))) {
        struct sockaddr_storage saddr_s;
        len_saddr = sizeof(saddr_s);
        getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
        if (saddr_s.ss_family == AF_INET) {
            printf("[TCP server] IPv4 Port : #%d\n",
                    ntohs(((struct sockaddr_in *)&saddr_s)->sin_port));
        } else if (saddr_s.ss_family == AF_INET6) {
            printf("[TCP server] IPv6 Port : #%d\n",
                    ntohs(((struct sockaddr_in6 *)&saddr_s)->sin6_port));
        } else {
            printf("[TCP server] (ss_family=%d)\n", saddr_s.ss_family);
        }
    }

    listen(fd_listener, LISTEN_BACKLOG);

    for (i = 0; i < MAX_POOL; i++) {
        switch (pid = fork()) {
            case 0:
                start_child(fd_listener, i);
                exit(EXIT_SUCCESS);

            case -1:
                perror("[TCP server] Fail: fork()");
                break;

            default:
                printf("[TCP server] Making child process No.%d \n", i);
                break;
        }
    }

    for (;;)
        pause();
    return 0;
}

void start_child(int sfd, int idx)
{
    int cfd, rc_gai;
    socklen_t len_saddr;
    char addrstr[INET6_ADDRSTRLEN], portstr[8];
    struct sockaddr_storage saddr_c;

    while (1) {
        len_saddr = sizeof(saddr_c);
        if ((cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
            perror("accept");
            close(cfd);
            continue;
        }

        rc_gai = getnameinfo((struct sockaddr *)&saddr_c, len_saddr, addrstr, sizeof(addrstr),
                portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);

        if (rc_gai) {
            fprintf(stderr, "Fail: getnameinfo(): %s\n", gai_strerror(rc_gai));
            exit(EXIT_FAILURE);
        }

        if (saddr_c.ss_family == AF_INET) {
            printf("[Child:%d] accept IPv4 (ip:port) (%s:%s) \n", idx, addrstr, portstr);
        } else if (saddr_c.ss_family == AF_INET6) {
            printf("[Child:%d] accept IPv6 (ip:port,scope) (%s:%s,%d) \n", idx, addrstr, portstr,
                    ((struct sockaddr_in6 *)&saddr_c)->sin6_scope_id);
        }

        recv_file(cfd, idx);
    }
}

int recv_file(int cfd, int idx)
{
    ssize_t ret_len, ret_read;
    char buf[BUFSIZ], filename[PATH_MAX] = { 0, };
    int fd, flag;

    while (1) {
        ret_len = recv(cfd, filename, PATH_MAX - 1, 0);
        if (ret_len == -1) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "[Child:%d] Fail: recv(): %s \n", idx, strerror(errno));
            return -1;
        }
        break;
    }
    printf("[Child:%d] File name : (%s) \n", idx, filename);

    if ((fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1) {
        // fail to open file
        fprintf(stderr, "[Child:%d] Fail: open()\n", idx);
        flag = -1;
        send(cfd, &flag, sizeof(flag), 0);
        close(cfd);
        return -1;
    } else {
        flag = 0;
        send(cfd, &flag, sizeof(flag), 0);
        printf("[Child:%d] File receiving... \n", idx);
    }

    while (1) {
        ret_len = recv(cfd, buf, BUFSIZ, 0);
        if (ret_len == -1) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "[Child:%d] Fail: recv(): %s \n", idx, strerror(errno));
            close(cfd);
            close(fd);
            return -1;
        } else if (ret_len == 0) {
            printf("[Child:%d] Complete to receive the file\n", idx);
            break;
        }
        
        while (ret_len) {
            // file write
            if ((ret_read = write(fd, buf, ret_len)) == -1)
                continue;
            ret_len -= ret_read;
        }
    }

    close(fd);
    close(cfd);
    return 0;
}
