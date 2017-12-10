#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include "stdalsp.h"

#define LISTEN_BACKLOG	256
#define MAX_FD_SOCKET	0xff

struct pollfd	pollfds[MAX_FD_SOCKET];
int		cnt_fd_socket;

int add_socket(int fd);
int del_socket(int fd);

int main(int argc, char *argv[])
{
	socklen_t	len_saddr;
	int		i, fd, fd_listener, ret_recv, ret_poll;
	char	*port, buf[1024];

	if (argc != 2)
		printf("%s [port number] \n", argv[0]);
	if (argc == 2)
		port = strdup(argv[1]);
	else
		port = strdup("0");

	struct addrinfo	ai, *ai_ret;
	int		rc_gai;

	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_INET;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

	if ((rc_gai = getaddrinfo(NULL, port, &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	if ((fd_listener = socket(ai_ret->ai_family,
					ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1) {
		pr_err("Fail: socket()");
		exit(EXIT_FAILURE);
	}

	int sockopt = 1;

	if (setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1) {
		pr_err("Fail: setsockopt()");
		exit(EXIT_FAILURE);
	}
	
	if (fcntl(fd_listener, F_SETFL, O_NONBLOCK) == -1) {
		pr_err("Fail: fcntl() error");
		exit(EXIT_FAILURE);
	}

	if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("Fail: bind()");
		exit(EXIT_FAILURE);
	}

	if (!strncmp(port, "0", strlen(port))) {
		struct sockaddr_storage	saddr_s;
		len_saddr = sizeof(saddr_s);

		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);

		if (saddr_s.ss_family == AF_INET) {
			pr_out("bind : IPv4 Port : #%d",
					ntohs(((struct sockaddr_in *)&saddr_s)->sin_port));
		} else if (saddr_s.ss_family == AF_INET6) {
			pr_out("bind : IPv6 Port : #%d",
					ntohs(((struct sockaddr_in6 *)&saddr_s)->sin6_port));
		} else {
			pr_out("getsockname : ss_family = %d", saddr_s.ss_family);
		}
	} else {
		pr_out("bind : %s", port);
	}

	listen(fd_listener, LISTEN_BACKLOG);

	add_socket(fd_listener);

	while (1) {
		if ((ret_poll = poll(pollfds, cnt_fd_socket, -1)) == -1) {
			pr_err("Fail: poll()");
			exit(EXIT_FAILURE);
		}

		pr_out("\tpoll = (%d)", ret_poll);
		if (pollfds[0].revents & POLLIN) {
			struct sockaddr_storage saddr_c;

			while (1) {
				len_saddr = sizeof(saddr_c);
				if ((fd = accept(pollfds[0].fd, (struct sockaddr *)&saddr_c, &len_saddr)) == -1) {
					if (errno == EAGAIN) {
						break;
					}
					pr_err("Error get connection from listen socket");
					break;
				}
				if (add_socket(fd) == -1) {
					/* error */
				}
				pr_out("accept : add socket(%d)", fd);
			}
			continue;
		}

		for (i = 1; i < cnt_fd_socket && ret_poll > 0; ++i) {
			if (pollfds[i].revents & POLLIN) {
				pr_out("POLLIN : normal-inband");
				if ((ret_recv = recv(pollfds[i].fd, buf, sizeof(buf), 0)) == -1) {
					pr_err("fd(%d) recv() error (%s)", pollfds[i].fd, strerror(errno));
				} else {
					if (ret_recv == 0) {
						pr_out("fd(%d) : Session closed", pollfds[i].fd);
						del_socket(pollfds[i].fd);
						i--;
					} else {
						pr_out("recv(fd=%d,n=%d) = %.*s", pollfds[i].fd, ret_recv, ret_recv, buf);
					}
				}

				ret_poll--;
#ifdef ENABLE_MSG_OOB
			} else if (pollfds[i].revents & POLLPRI) {
				pr_out("POLLPRI : Urgent data detected");
				if ((ret_recv = recv(pollfds[i].fd, buf, 1, MSG_OOB)) == -1) {
					pr_err("Fail: recv()");
				}
				pr_out("recv(fd=%d,n=1) = %.*s (OOB)", pollfds[i].fd, 1, buf);
#endif
			} else if (pollfds[i].revents & POLLERR) {
				ret_poll--;
			} else if (pollfds[i].revents & POLLNVAL) {
				ret_poll--;
			} else {
				pr_out("> No signal:fd(%d)", pollfds[i].fd);
			}
		}
	}

	return 0;
}

int add_socket(int fd)
{
	if (cnt_fd_socket < MAX_FD_SOCKET) {
		pollfds[cnt_fd_socket].fd = fd;
#ifdef ENABLE_MSG_OOB
		pollfds[cnt_fd_socket].events = POLLIN | POLLPRI;
#else
		pollfds[cnt_fd_socket].events = POLLIN;
#endif
		return ++cnt_fd_socket;
	} else {
		return -1;
	}
}

int del_socket(int fd)
{
	int		i, flag = 0;
	close(fd);

	for (i = 0; i < cnt_fd_socket; i++) {
		if (pollfds[i].fd == fd) {
			if (i != (cnt_fd_socket - 1)) {
				pollfds[i] = pollfds[cnt_fd_socket - 1];
			}

			pollfds[cnt_fd_socket - 1].fd = -1;
			flag = 1;
			break;
		}
	}

	if (flag == 0) {
		return -1;
	}

	--cnt_fd_socket;
	return i;
}
