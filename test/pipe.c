#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	pid_t pid;
	int fd[2];

	if ((pipe(fd) == -1) || ((pid = fork()) == -1)) {
		perror("Failed to pipe");
		return 1;
	}

	switch (pid) {
		case 0:
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				perror("Fail to redirect to stdout");
			else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
				perror("Fail to close");
			else {
				execl("/bin/ls", "ls", "-l", NULL);
				perror("Fail to exec");
			}
			return 1;
		default:
			if (dup2(fd[0], STDIN_FILENO) == -1)
				perror("Fail to redirect to stdin");
			else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
				perror("Fail to close");
			else {
				execl("/usr/bin/sort", "sort", "-k", "+4", NULL);
				perror("Fail to exec");
			}
			return 1;
	}
	return 0;
}
