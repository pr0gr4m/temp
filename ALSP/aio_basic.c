#include <stdio.h>
#include <errno.h>
#include <aio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define FILENAME	"fd_test.log"
#define TEST_MSG	"[Test message:1234567890]\n"

int main()
{
	int fd, ret;
	struct aiocb aio_wb;

	if ((fd = open(FILENAME, O_CREAT | O_RDWR, 0644)) == -1) {
		exit(EXIT_FAILURE);
	}
	write(fd, TEST_MSG, sizeof(TEST_MSG));
	memset(&aio_wb, 0, sizeof(struct aiocb));
	aio_wb.aio_fildes = fd;
	aio_wb.aio_buf = TEST_MSG;
	aio_wb.aio_nbytes = sizeof(TEST_MSG);
	aio_wb.aio_offset = 5;
	aio_write(&aio_wb);
	while ((ret = aio_error(&aio_wb)) != 0) {
		if (ret == EINPROGRESS) {
			printf("aio_write has not been complted. sleep(1) \n");
			sleep(1);
		} else {
			printf("Error: aio_error = %d\n", ret);
			break;
		}
	}

	if ((ret = aio_return(&aio_wb)) == -1) {
		fprintf(stderr, "Error: aio_write: %s\n", strerror(errno));
	}

	printf("> wrtie complete:%d B\n", ret);
	fsync(fd);
	close(fd);
	return EXIT_SUCCESS;
}
