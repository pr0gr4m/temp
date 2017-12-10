#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAXLINE	511
#define MAXSOCK	1024

char *exit_string = "exit";
char *start_string = "connected to server\n";

int maxfdp1;
int num_chat = 0;
int clisock_list[MAXSOCK];
int listen_sock;

void addclient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeclient(int s);
int tcp_listen(int host, int sort, int backlog);
void errquit(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in cliaddr;
	char buf[MAXLINE + 1];
	int i, j, nbyte, acpt_sock;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	fd_set read_fds;

	if (argc != 2) {
		printf("Usage: %s <port> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		for (i = 0; i < num_chat; i++)
			FD_SET(clisock_list[i], &read_fds);

		maxfdp1 = getmax() + 1;
		puts("wait for client");
		if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0)
			errquit("select fail");
		if (FD_ISSET(listen_sock, &read_fds)) {
			acpt_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
			if (acpt_sock == -1)
				errquit("accept");
			addclient(acpt_sock, &cliaddr);
			send(acpt_sock, start_string, strlen(start_string), 0);
			printf("%d th user addition \n", num_chat);
		}

		for (i = 0; i < num_chat; i++) {
			if (FD_ISSET(clisock_list[i], &read_fds)) {
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
				if (nbyte == 0) {
					removeclient(i);
					continue;
				}
				buf[nbyte] = 0;
				if (strstr(buf, exit_string) != NULL) {
					removeclient(i);
					continue;
				}
				for (j = 0; j < num_chat; j++)
					send(clisock_list[j], buf, nbyte, 0);
				printf("%s \n", buf);
			}
		}
	}

	return 0;
}

void addclient(int s, struct sockaddr_in *newcliaddr)
{
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client : %s \n", buf);
	clisock_list[num_chat++] = s;
}

void removeclient(int s)
{
	close(clisock_list[s]);
	if (s != num_chat - 1)
		clisock_list[s] = clisock_list[num_chat - 1];
	num_chat--;
	printf("Logouted one of the user: %d \n", num_chat);
}

int getmax()
{
	int max = listen_sock;
	for (int i = 0; i < num_chat; i++)
		if (clisock_list[i] > max)
			max = clisock_list[i];
	return max;
}

int tcp_listen(int host, int port, int backlog)
{
	int sd;
	struct sockaddr_in servaddr;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);

	if (bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	listen(sd, backlog);
	return sd;
}
