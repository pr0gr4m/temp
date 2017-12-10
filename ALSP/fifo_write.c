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
	int		fd, rc_write, rc_getline;
	char	*p_buf = NULL;
	size_t	len_buf = 0;

	if ((fd = open(PATH_FIFO, O_WRONLY, 0644)) == -1) {
		perror("open() error");
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("To FIFO >> ");
		fflush(stdout);
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) {
			perror("getline() error");
			return EXIT_FAILURE;
		}

		if (p_buf[rc_getline - 1] == '\n') {
			p_buf[rc_getline - 1] = 0;
			rc_getline--;
		}
		if ((rc_write = write(fd, p_buf, strlen(p_buf))) == -1) {
			perror("write() error");
		}

		printf("* Writing %d bytes... \n", rc_write);
		free(p_buf);
		p_buf = NULL;
	}

	return (EXIT_SUCCESS);
}


