#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char msg[BUFSIZ];
	int str_len;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	if (argc != 2) {
		printf("Usage: %s <port> \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen");

	while (1) {
		socklen_t clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

		if (clnt_sock == -1) {
			error_handling("accept");
		}

		while ((str_len = recv(clnt_sock, msg, BUFSIZ, 0)) != 0 && str_len != -1) {
			send(clnt_sock, msg, str_len, 0);
			write(1, msg, str_len);
		}
		close(clnt_sock);
	}

	return 0;
}
