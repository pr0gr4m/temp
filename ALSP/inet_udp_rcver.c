#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "stdalsp.h"

int main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_s, saddr_c;
	int		sd, len_recv, len_send;
	socklen_t	len_saddr;
	short	port;
	char	rbuf[1024];

	memset(&saddr_s, 0x00, sizeof(saddr_s));
	memset(&saddr_c, 0x00, sizeof(saddr_c));

	if (argc > 2) {
		printf("%s <port>", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
		port = (short) atoi((char *)argv[1]);
	else
		port = 0;

	if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
		pr_err("[UDP Receiver] Fail: socket()");
		exit(EXIT_FAILURE);
	}

	saddr_s.sin_family = AF_INET;
	saddr_s.sin_addr.s_addr = INADDR_ANY;
	saddr_s.sin_port = htons(port);

	if (bind(sd, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("[UDP Receiver] Fail: bind()");
		exit(EXIT_FAILURE);
	}

	if (port == 0) {
		len_saddr = sizeof(saddr_s);
		getsockname(sd, (struct sockaddr *)&saddr_s, &len_saddr);
	}

	pr_out("[UDP Receiver] Port : #%d", ntohs(saddr_s.sin_port));

	while (1) {
		len_saddr = sizeof(saddr_c);
		len_recv = recvfrom(sd, rbuf, sizeof(rbuf), 0,
				(struct sockaddr *)&saddr_c, &len_saddr);

		if (len_recv == -1) {
			pr_err("[UDP Receiver] Fail: recvfrom()");
		}

		pr_out("[recvfrom] (%d byte) (%.*s)", len_recv, len_recv, rbuf);
		pr_out("[sendto  ] (%s:%d)",
				inet_ntoa(saddr_c.sin_addr), ntohs(saddr_c.sin_port));
		len_send = sendto(sd, rbuf, len_recv, 0,
				(struct sockaddr *)&saddr_c, sizeof(saddr_c));

		if (len_send == -1) {
			pr_err("[UDP Receiver] Fail: sendto()");
		}
	}

	return 0;
}
