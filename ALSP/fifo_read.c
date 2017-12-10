#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PATH_FIFO	"/tmp/my_fifo"

int main()
{
	int		fd, n_read = 0;
	char	a_buf[0xff];
	char	err_buf[128];

	if (mkfifo(PATH_FIFO, 0644) == -1) {
		if (errno != EEXIST) {
			strerror_r(errno, err_buf, sizeof(err_buf));
			fprintf(stderr, "FAIL: mkfifo() error (%s) [%s:%d]\n", err_buf, __FUNCTION__, __LINE__);
			exit(EXIT_FAILURE);
		}
	}

	if ((fd = open(PATH_FIFO, O_RDONLY, 0644)) == -1) {
		perror("open() error");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((n_read = read(fd, a_buf, sizeof(a_buf))) == -1) {
			perror("read() error");
			exit(EXIT_FAILURE);
		}

		if (n_read == 0) {
			printf("broken pipe\n");
			exit(0);
		}

		printf("[%1$d byte] %2$.*1$s\n", n_read, a_buf);
	}

	return (EXIT_SUCCESS);
}
