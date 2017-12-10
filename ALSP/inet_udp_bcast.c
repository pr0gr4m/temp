#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "stdalsp.h"

int main(int argc, char *argv[])
{
	struct addrinfo ai, *ai_ret;
	int		udp_fd, sockopt, sz_slen;
	char	a_sbuf[50];

	if (argc != 2) {
		printf("%s <port>", argv[0]);
		exit(EXIT_FAILURE);
	}
	char	*port = strdup(argv[1]);

	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_INET;
	ai.ai_socktype = SOCK_DGRAM;
	ai.ai_flags = AI_ADDRCONFIG;

	int rc_gai;

	if ((rc_gai = getaddrinfo("255.255.255.255", port, &ai, &ai_ret)) != 0) {
		pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
		exit(EXIT_FAILURE);
	}

	udp_fd = socket(ai_ret->ai_family, ai_ret->ai_socktype, ai_ret->ai_protocol);

	if (udp_fd == -1) {
		pr_err("[UDP broadcast] Fail: socket()");
		exit(EXIT_FAILURE);
	}

	sockopt = 1;
	socklen_t	len_sockopt = sizeof(sockopt);

	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &sockopt, len_sockopt) == -1) {
		pr_err("[UDP broadcast] Fail: setsockopt()");
		exit(EXIT_FAILURE);
	}

	sprintf(a_sbuf, "%s", "UDP broadcasting test");
	pr_out("- Send broadcasting data every 3 sec");

	while (1) {
		sz_slen = sendto(udp_fd, a_sbuf, strlen(a_sbuf),
				0, ai_ret->ai_addr, ai_ret->ai_addrlen);
		if (sz_slen == -1) {
			pr_err("[UDP broadcast] Fail: sento()");
		} else {
			printf("<< send broadcasting msg\n");
		}
		sleep(3);
	}

	return 0;
}
