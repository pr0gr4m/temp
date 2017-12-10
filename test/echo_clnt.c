#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int sock;
	char msg[BUFSIZ];
	int str_len;
	struct sockaddr_in serv_addr;


	if (argc != 3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect");

	while (1) {
		fputs(">> ", stdout);
		fgets(msg, BUFSIZ, stdin);

		if (!strcmp(msg, "q\n"))
			break;
		send(sock, msg, strlen(msg), 0);

		str_len = recv(sock, msg, BUFSIZ - 1, 0);
		msg[str_len] = 0;
		printf("msg from serv : %s\n", msg);
	}

	close(sock);
	return 0;
}
	
