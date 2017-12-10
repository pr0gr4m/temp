#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include "stdalsp.h"

#define LISTEN_BACKLOG	256
#define ADD_EV(a, b)	if (add_ev(a, b) == -1) { pr_err("Fail: add_ev"); exit(1); }
#define DEL_EV(a, b)	if (del_ev(a, b) == -1) { pr_err("Fail: del_ev"); exit(1); }

const int	max_ep_events = 256;
int		epollfd;

int add_ev(int efd, int fd);
int del_ev(int efd, int fd);
int fcntl_setnb(int fd);

int main(int argc, char *argv[])
{
	socklen_t	len_saddr;
	int		fd, fd_listener;
	int		i, ret_recv, ret_poll;
	char	*port, buf[1024];
	struct epoll_event	*ep_events;

	if (argc > 2) {
		printf("%s [port number] \n", argv[0]);
		exit(EXIT_FAILURE);
	}

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

	fcntl_setnb(fd_listener);
	if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1) {
		pr_err("Fail: bind()");
		exit(EXIT_FAILURE);
	}

	listen(fd_listener, LISTEN_BACKLOG);

	if ((epollfd = epoll_create(1)) == -1) {
		pr_err("Fail: epoll_create()");
		exit(EXIT_FAILURE);
	}

	if ((ep_events = calloc(max_ep_events, sizeof(struct epoll_event))) == NULL) {
		pr_err("Fail: calloc() epoll_event");
		exit(EXIT_FAILURE);
	}

	ADD_EV(epollfd, fd_listener);

	while (1) {
		pr_out("Epoll waiting...");

		if ((ret_poll = epoll_wait(epollfd, ep_events, max_ep_events, -1)) == -1) {
			pr_err("epoll_wait error");
		}

		pr_out("Epoll return (%d)", ret_poll);
		for (i = 0; i < ret_poll; i++) {
			if (ep_events[i].events & EPOLLIN) {
				if (ep_events[i].data.fd == fd_listener) {
					struct sockaddr_storage	saddr_c;

					while (1) {
						len_saddr = sizeof(saddr_c);
						fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
						if (fd == -1) {
							if (errno == EAGAIN) {
								break;
							}
							pr_err("Error get connection from listen socket");
							break;
						}
						fcntl_setnb(fd);
						ADD_EV(epollfd, fd);
						pr_out("accept : add socket (%d)", fd);
					}
					continue;
				}

				if ((ret_recv = recv(ep_events[i].data.fd, buf, sizeof(buf), 0)) == -1) {
					/* error */
				} else {
					if (ret_recv == 0) {
						pr_out("fd(%d) : Session closed", ep_events[i].data.fd);
						DEL_EV(epollfd, ep_events[i].data.fd);
					} else {
						pr_out("recv(fd=%d,n=%d) = %.*s",
								ep_events[i].data.fd, ret_recv, ret_recv, buf);
					}
				}
			} else if (ep_events[i].events & EPOLLPRI) {
				pr_out("EPOLLPRI : Urgent data detected");
				if ((ret_recv = recv(ep_events[i].data.fd, buf, 1, MSG_OOB)) == -1) {
					/* error */
				}
				pr_out("recv(fd=%d,n=1) = %.*s", ep_events[i].data.fd, 1, buf);
			} else if (ep_events[i].events & EPOLLERR) {
				/* error */
			} else {
				pr_out("fd(%d) epoll event(%d) err(%s)",
						ep_events[i].data.fd, ep_events[i].events, strerror(errno));
			}
		}
	}

	return 0;
}

int add_ev(int efd, int fd)
{
	struct epoll_event	ev;

	ev.events = EPOLLIN | EPOLLPRI;
	ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		pr_out("fd(%d) EPOLL_CTL_ADD Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	return 0;
}

int del_ev(int efd, int fd)
{
	if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		pr_out("fd(%d) EPOLL_CTL_DEL Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}

int fcntl_setnb(int fd)
{
	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1) {
		return errno;
	}
	return 0;
}
