#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <errno.h>
#include "stdalsp.h"

#define PATH_UNIX_SOCKET	"/tmp/my_socket"

int main()
{
	struct sockaddr_un	saddr_u = {};
	char	*buf;
	size_t	len;
	ssize_t	read;
	int		fd, ret_len;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, IPPROTO_IP)) == -1) {
		pr_err("[TCP client] Fail: socket()");
		exit(EXIT_FAILURE);
	}
	
	/*
	if (remove(PATH_UNIX_SOCKET)) {
		if (errno != ENOENT) {
			pr_err("[TCP Socket] Fail: remove()");
			exit(EXIT_FAILURE);
		}
	} */

	saddr_u.sun_family = AF_UNIX;
	snprintf(saddr_u.sun_path, sizeof(saddr_u.sun_path), PATH_UNIX_SOCKET);

	if (connect(fd, (struct sockaddr *)&saddr_u, sizeof(saddr_u)) == -1) {
		pr_err("[TCP client] Fail: connect()");
		exit(EXIT_FAILURE);
	}

	pr_out("[UNIX Domain] PATH: #%s", PATH_UNIX_SOCKET);

	for (;;) {
		if ((read = getline(&buf, &len, stdin)) == -1) {
			perror("getline error");
			continue;
		}

		if (send(fd, buf, read - 1, 0) == -1) {
			pr_err("[TCP client] Fail: send()");
			close(fd);
		}
		ret_len = recv(fd, buf, sizeof(buf), 0);

		if (ret_len == -1) {
			if (errno == EINTR)
				continue;
			pr_err("[TCP client] Fail: recv()");
			break;
		} else if (ret_len == 0) {
			pr_err("[TCP client] Session closed");
			close(fd);
			break;
		}
		pr_out("[TCP client] RECV(%.*s)", ret_len, buf);
	}
	free(buf);
	return 0;
}
