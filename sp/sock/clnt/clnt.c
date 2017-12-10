#include "common.h"

int send_file(int sfd, int fd, char filename[]);

int main(int argc, char *argv[])
{
    int sfd, fd, rc_gai;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc != 5) {
        printf("%s <hostname> <port> <filename> <SO_REUSEADDR off(0) on(non-zero)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[3], O_RDONLY)) == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage sae_local;
    socklen_t len_sae_local = sizeof(sae_local);
    char addrstr[INET_ADDRSTRLEN], portstr[8];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;

    if ((rc_gai = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
        fprintf(stderr, "Fail: getaddrinfo(): %s\n", gai_strerror(rc_gai));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);

        if (sfd == -1)
            continue;

        if (argv[4][0] != '0') {
            int sockopt = 1;
            if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1) {
                perror("setsockopt()");
                exit(EXIT_FAILURE);
            }
            printf("Socket option = SO_REUSEADDR(on)\n");
        } else {
            printf("Socket option = SO_REUSEADDR(off)\n");
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;  // success

        close(sfd);
    }

    if (rp == NULL) {   // No address succeeded
        fprintf(stderr, "Could not connect \n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    if (getsockname(sfd, (struct sockaddr *)&sae_local, &len_sae_local) == -1) {
        perror("Fail: getsockname()");
        exit(EXIT_FAILURE);
    }

    if ((rc_gai = getnameinfo((struct sockaddr *)&sae_local, len_sae_local,
                    addrstr, sizeof(addrstr), portstr, sizeof(portstr),
                    NI_NUMERICHOST | NI_NUMERICSERV))) {
        perror("getnameinfo()");
        exit(EXIT_FAILURE);
    }

    printf("Connection established\n");
    printf("\tLocal(%s:%s) => Destination(%s:%s) \n", addrstr, portstr, argv[1], argv[2]);
    send_file(sfd, fd, argv[3]);
    return 0;
}


int send_file(int sfd, int fd, char filename[])
{
    char buf[BUFSIZ];
    ssize_t ret_send, ret_read;
    int flag;

    if ((ret_send = send(sfd, filename, strlen(filename), 0)) != strlen(filename)) {
        perror("Fail: send()");
        goto ERROR;
    }

    if (recv(sfd, &flag, sizeof(flag), 0) == -1) {
        perror("Fail: recv()");
        goto ERROR;
    }

    if (flag == -1) {
        // Fail to create file
        fprintf(stderr, "Fail to create file to server \n");
        goto ERROR;
    }

    printf("\t[%s] Sending file...\n", filename);
    while (1) {
        // read the file
        if ((ret_read = read(fd, buf, BUFSIZ)) == -1) {
            perror("Fail: read()");
            goto ERROR;
        }

        if (ret_read == 0) {
            // end of file
            printf("Complete to send! \n");
            break;
        }

        while (ret_read) {
            // send the file
            if ((ret_send = send(sfd, buf, ret_read, 0)) == -1) {
                perror("Fail: send()");
                goto ERROR;
            }
            ret_read -= ret_send;
        }
    }

    close(sfd);
    close(fd);
    return 0;

ERROR:
    close(sfd);
    close(fd);
    exit(EXIT_FAILURE);
}
