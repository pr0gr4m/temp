#include "apue.h"
#include <sys/wait.h>

#define BUF_SIZE 1024
#define SHELL	"/bin/bash"

void seek_var(void);
void print_var(void);

int
main(void)
{
	int n;
	int fd[2];
	pid_t pid;
	char buf[BUF_SIZE];

	if (pipe(fd) < 0)
		err_sys("pipe error");

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid > 0)
	{
		close(fd[1]);

		if (fd[0] != STDIN_FILENO)
		{
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");

			close(fd[0]);
		}

		if (waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");

		seek_var();
		print_var();
	}
	else
	{
		close(fd[0]);

		if (fd[1] != STDOUT_FILENO)
		{
			if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error to stdout");
			close(fd[1]);
		}

		if (execl(SHELL, SHELL, "set", (char *)0) < 0)
			err_sys("execl error for %s", SHELL);
	}
}

void
seek_var()
{
	int cnt = 0;
	char buf[BUF_SIZE];

	while (fgets(buf, BUF_SIZE, stdin) != NULL)
	{
		if (cnt++ > 103)
			break;
	}
}

void
print_var(void)
{
	char buf[BUF_SIZE];

	while (fgets(buf, BUF_SIZE, stdin) != NULL)
		fputs(buf, stdout);
}
