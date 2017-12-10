#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "stdalsp.h"

int main(int argc, char *argv[])
{
	int		fd, rc_gai, flag_once = 0;
	struct addrinfo	ai_dest, *ai_dest_ret;

	if (argc != 4) {
		printf("%s <hostname> <port> <SO_REUSEADDR off(0) on(non-zero)>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_storage sae_local;
	socklen_t	len_sae_local = sizeof(sae_local);
	char	addrstr[INET_ADDRSTRLEN], portstr[8];

	for (int i = 0; ; i++) {
		memset(&ai_dest, 0, sizeof(ai_dest));
		ai_dest.ai_family = AF_UNSPEC;
		ai_dest.ai_socktype = SOCK_STREAM;
		ai_dest.ai_flags = AI_ADDRCONFIG;

		if ((rc_gai = getaddrinfo(argv[1], argv[2], &ai_dest, &ai_dest_ret)) != 0) {
			pr_err("Fail: getaddrinfo():%s", gai_strerror(rc_gai));
			exit(EXIT_FAILURE);
		}

		if ((fd = socket(ai_dest_ret->ai_family,
						ai_dest_ret->ai_socktype,
						ai_dest_ret->ai_protocol)) == -1) {
			pr_err("[Client] Fail: socket()");
			exit(EXIT_FAILURE);
		}

		if (argv[3][0] != '0') {
			int sockopt = 1;
			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1) {
				exit(EXIT_FAILURE);
			}
			printf("Socket option = SO_REUSEADDR(on)\n");
		} else {
			printf("Socket option = SO_REUSEADDR(off)\n");
		}

		if (flag_once) {
			if (bind(fd, (struct sockaddr *)&sae_local, len_sae_local) == -1) {
				pr_err("Fail: bind():%s", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		if (connect(fd, ai_dest_ret->ai_addr, ai_dest_ret->ai_addrlen) == -1) {
			pr_err("Fail: connect()");
			exit(EXIT_FAILURE);
		}

		if (flag_once == 0) {
			if (getsockname(fd, (struct sockaddr *)&sae_local, &len_sae_local) == -1) {
				pr_err("Fail: getpeername()");
				exit(EXIT_FAILURE);
			}

			if ((rc_gai = getnameinfo((struct sockaddr *)&sae_local, len_sae_local,
							addrstr, sizeof(addrstr), portstr, sizeof(portstr),
							NI_NUMERICHOST | NI_NUMERICSERV))) {
				pr_err("Fail: getnameinfo()");
				exit(EXIT_FAILURE);
			}
		}

		flag_once = 1;
		printf("Connection established\n");
		printf("\tLocal(%s:%s) => Destination(%s:%s)\n", addrstr, portstr, argv[1], argv[2]);
		printf(">> Press any key to disconnect.");
		getchar();
		close(fd);
		printf(">> Press any key to reconnect.");
		getchar();
	}

	return 0;
}
