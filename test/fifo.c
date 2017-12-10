#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

int child(const char *fifoname, const char *idstring);
int parent(const char *fifoname);

int main(int argc, char *argv[])
{
	pid_t pid;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <fifoname> \n", argv[0]);
		return 1;
	}

	if (mkfifo(argv[1], 0600) == -1) {
		if (errno != EEXIST) {
			perror("mkfifo");
			return 1;
		}
	}

	switch (pid = fork()) {
		case -1:
			perror("fork");
			return 1;

		case 0:
			return child(argv[1], "written by child");

		default:
			return parent(argv[1]);
	}

	return 0;
}

int child(const char *fifoname, const char *idstring)
{
	char buf[BUFSIZ];
	int fd, rval;
	ssize_t strsize;

	printf("[Child:%ld] About to open FIFO %s...\n", (long)getpid(), fifoname);
	while (((fd = open(fifoname, O_WRONLY)) == -1) && (errno == EINTR));
	if (fd == -1) {
		perror("open");
		return 1;
	}

	rval = snprintf(buf, BUFSIZ, "[%ld]: %s\n", (long)getpid(), idstring);

	if (rval < 0) {
		perror("snprintf");
		return 1;
	}

	strsize = strlen(buf) + 1;
	printf("[Child:%ld] About to write... \n", (long)getpid());
	if ((rval = write(fd, buf, strsize)) != strsize) {
		perror("write");
		return 1;
	}

	printf("[Child:%ld] Finished... \n", (long)getpid());
	return 0;
}


int parent(const char *fifoname)
{
	char buf[BUFSIZ];
	int fd, rval;

	printf("[Parent:%ld] About to open FIFO: %s...\n", (long)getpid(), fifoname);

	while (((fd = open(fifoname, 0600)) == -1) && (errno == EINTR));
	if (fd == -1) {
		perror("open");
		return 1;
	}

	printf("[Parent:%ld] About to read... \n", (long)getpid());
	
	if ((rval = read(fd, buf, BUFSIZ)) == -1) {
		perror("read");
		return 1;
	}

	printf("[Parent:%ld]: read %.*s \n", (long)getpid(), rval, buf);
	return 0;
}
