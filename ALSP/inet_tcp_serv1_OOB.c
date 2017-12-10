#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "stdalsp.h"

#define LISTEN_BACKLOG	20
#define MAX_POOL	3

int fd_listener;

void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int		i, rc_gai;
	socklen_t	len_saddr;
	pid_t	pid;
	struct addrinfo	ai, *ai_ret;

	if (argc != 2) {
		printf("%s [port number] \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;
	
	if ((rc_gai = getaddrinfo(NULL, argv[1], &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	if ((fd_listener = socket(
					ai_ret->ai_family, ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1) {
		pr_err("[TCP server] Fail: socket()");
		exit(EXIT_FAILURE);
	}

	if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("[TCP server] Fail: bind()");
		exit(EXIT_FAILURE);
	}

	if (!strncmp(argv[1], "0", strlen(argv[1]))) {
		struct sockaddr_storage	saddr_s;
		len_saddr = sizeof(saddr_s);
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);

		if (saddr_s.ss_family == AF_INET) {
			pr_out("[TCP server] IPv4 Port : #%d",
					ntohs(((struct sockaddr_in *)&saddr_s)->sin_port));
		} else if (saddr_s.ss_family == AF_INET6) {
			pr_out("[TCP server] IPv6 Port : #%d",
					ntohs(((struct sockaddr_in6 *)&saddr_s)->sin6_port));
		} else {
			pr_out("[TCP server] (ss_family=%d)", saddr_s.ss_family);
		}
	}

	listen(fd_listener, LISTEN_BACKLOG);

	for (i = 0; i < MAX_POOL; i++) {
		switch (pid = fork()) {
			case 0:
				start_child(fd_listener, i);
				exit(EXIT_SUCCESS);
				break;
			case -1:
				pr_err("[TCP server] Fail: fork()");
				break;
			default:
				pr_out("[TCP server] Making child process No.%d", i);
				break;
		}
	}

	for (;;)
		pause();
	return 0;
}

void start_child(int sfd, int idx)
{
	int		cfd, ret_recv, cum_recv, ret_sockatmark;
	socklen_t	len_saddr;
	char	buf[0xff];
	struct sockaddr_storage	saddr_c;

	for (;;) {
		ret_recv = cum_recv = 0;
		len_saddr = sizeof(saddr_c);

		if ((cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
			pr_err("[Child] Fail: accept()");
			close(cfd);
			continue;
		}

		for (;;) {
			ret_sockatmark = sockatmark(cfd);
			if (ret_sockatmark == 0) { /* normal band */
WAIT_RECV:
				pr_out("[normal band] cum_recv (%d byte)", cum_recv);
				if ((ret_recv = recv(cfd, buf + cum_recv,
								sizeof(buf) - cum_recv, 0)) == -1) {
					pr_err("[Child:%d] Fail: recv(): %s", idx, strerror(errno));
				}
				
				if (ret_recv == 0) {
					pr_err("[Child:%d] Session closed", idx);
					close(cfd);
					break;
				}

				cum_recv += ret_recv;
			} else if (ret_sockatmark == 1) { /* OOB */
				pr_out("ret_sockatmark = %d", ret_sockatmark);
				if ((ret_recv = recv(cfd, buf + cum_recv, 2, MSG_OOB)) == -1) {
					if (errno == EINVAL) {
						pr_err("goto recv(norm-inband)");
						goto WAIT_RECV;
					}
					pr_err("ERR: OOB:ret_recv (%s) \n", strerror(errno));
					continue;
				}
				cum_recv++;
				pr_out("Recv(with OOB,%d bytes) [%.*s]\n", cum_recv, cum_recv, buf);
				cum_recv = 0;
			} else {
				pr_err("Fail: sockatmark : %s", strerror(errno));
			}
		}
	}
}
