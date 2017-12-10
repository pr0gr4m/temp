#include "apue.h"
#include <fcntl.h>

void seek_read(int fd, int seek);
void seek_write(int fd, int seek);

int
main(int argc, char *argv[])
{
	int fd;
	int seek;
	
	if (argc != 2)
		err_sys("usage: %s <file_descriptor>\n", argv[0]);

	fd = open(argv[1], O_RDWR);

	fputs("Where do you read? : ", stdout);
	scanf("%d", &seek);

	seek_read(fd, seek);

	fputs("Where do you write? : ", stdout);
	scanf("%d", &seek);
	getchar();

	seek_write(fd, seek);

	close(fd);
	return 0;
}

void seek_read(int fd, int seek)
{
	char buf;
	off_t cur_pos = lseek(fd, 0, SEEK_CUR);

	if (lseek(fd, seek, SEEK_SET) == -1)
		err_sys("lseek() error");

	if (read(fd, &buf, 1) < 0)
		err_sys("read() error");

	printf("readed character is %c \n", buf);
	
	if (lseek(fd, cur_pos, SEEK_SET) == -1)
		err_sys("lseek() error");
}

void seek_write(int fd, int seek)
{
	char buf;
	off_t cur_pos = lseek(fd, 0, SEEK_CUR);

	if (lseek(fd, seek, SEEK_SET) == -1)
		err_sys("lseek() error");

	fputs("What character do you write? : ", stdout);
	scanf("%c", &buf);

	if (write(fd, &buf, 1) != 1)
		printf("cannot write \n");
	else
		printf("write done \n");

	if (lseek(fd, cur_pos, SEEK_SET) == -1)
		err_sys("lseek() error");
}
