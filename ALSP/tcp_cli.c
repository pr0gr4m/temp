#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "stdalsp.h"
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>

#define str_ordinary	"1.abcde"
#define str_OOB			"2.fghij"

int main(int argc, char *argv[])
{
	int		fd, rc_gai;
	struct addrinfo	ai, *ai_ret;

	if (argc != 3) {
		printf("%s <hostname> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_flags = AI_ADDRCONFIG;

	if ((rc_gai = getaddrinfo(argv[1], argv[2], &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	if ((fd = socket(ai_ret->ai_family, ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1) {
		pr_err("[Client] Fail: socket()");
		exit(EXIT_FAILURE);
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL)) == -1) {
		pr_err("[Client] Fail: fcntl()");
		exit(EXIT_FAILURE);
	}

	(void)connect(fd, ai_ret->ai_addr, ai_ret->ai_addrlen);
	if (errno != EINPROGRESS) {
		pr_err("[Client] Fail: connect()");
		exit(EXIT_FAILURE);
	}

	fd_set	fdset_w;
	FD_ZERO(&fdset_w);
	FD_SET(fd, &fdset_w);

	if (select(fd + 1, NULL, &fdset_w, NULL, NULL) == -1) {
		pr_err("[Client] Fail: select()");
		exit(EXIT_FAILURE);
	}

	int		sockopt;
	socklen_t	len_sockopt = sizeof(sockopt);

	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &sockopt, &len_sockopt) == -1) {
		pr_err("[Client] Fail:getsockopt()");
		exit(EXIT_FAILURE);
	}

	if (sockopt) {
		pr_err("SO_ERROR: %s(%d)", strerror(sockopt), sockopt);
	}

	pr_out("[nonblocking] connection established");

	pr_out("[TCP client] : 1:ordinary data  2:OOB data  0:Exit program");

	int		rc_getline, rc_send, flag_send;
	char	*p_sbuf, *p_buf = NULL;
	size_t	len_buf;

	while (1) {
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) {
			return EXIT_FAILURE;
		}

		switch (atoi(p_buf)) {
			case 0:
				exit(EXIT_SUCCESS);
				break;

			case 1:
				p_sbuf = str_ordinary;
				flag_send = 0;
				printf(">> will send ordinary msg: data = [%s] \n", p_sbuf);
				break;

			case 2:
				p_sbuf = str_OOB;
				flag_send = MSG_OOB;
				printf(">> will send OOB msg: data = [%s] \n", p_sbuf);
				break;

			default:
				printf(">> Error : (%s) \n", p_sbuf);
				continue;
		}

		free(p_buf);
		p_buf = NULL;

		if ((rc_send = send(fd, p_sbuf, strlen(p_sbuf), flag_send)) == -1) {
			pr_err("[TCP client] Fail: send()");
		}
	}

	return 0;
}
