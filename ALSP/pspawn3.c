#include <stdio.h>
#include <spawn.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int ret_err = 0;
	pid_t pid_child;
	char buf_err[64];
	char *argv_child[] = { "forkexec_child", NULL };
	printf("Parent[%d]: Start \n", getpid());
	posix_spawnattr_t posix_attr;
	
	if ((ret_err = posix_spawnattr_init(&posix_attr)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_init: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	if ((ret_err = posix_spawnattr_setflags(&posix_attr, POSIX_SPAWN_SETPGROUP)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setflags: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	pid_t pid_pgid = 0;	/* 0이면 자식 프로세스는 독립하여 프로세스 그룹 리더가 된다 */
	if ((ret_err = posix_spawnattr_setpgroup(&posix_attr, pid_pgid)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setpgroup: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	ret_err = posix_spawn( &pid_child,
			argv_child[0],
			NULL,
			&posix_attr,
			argv_child,
			NULL);
	if ((ret_err = posix_spawnattr_destroy(&posix_attr)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_destroy: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}
	printf("Parent[%d]: Wait for child(%d) \n", getpid(), (int)pid_child);
	(void)wait(NULL);
	printf("Parent[%d]: Exit\n", getpid());
	return 0;
}
