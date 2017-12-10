#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int ret_err = 0, i = 0, i_child = 0;
	pid_t pid_child[16];
	char buf_err[64];
	char *argv_child_x[] = { "pspawn5_child_x", NULL };
	char *argv_child_y[] = { "pspawn5_child_y", NULL };

	printf("Parent[%d]: Start\n", getpid());
	sigset_t sigset_block;
	sigemptyset(&sigset_block);
	sigaddset(&sigset_block, SIGUSR1);
	sigaddset(&sigset_block, SIGUSR2);
	sigaddset(&sigset_block, SIGTERM);
	sigprocmask(SIG_SETMASK, &sigset_block, NULL);

	posix_spawnattr_t posix_attr;
	if ((ret_err = posix_spawnattr_init(&posix_attr)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_init: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	short posix_flags = POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK;
	if ((ret_err = posix_spawnattr_setflags(&posix_attr, posix_flags)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setflags: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	sigset_t sigset_mask;
	/* 1st group */
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGUSR2);
	if ((ret_err = posix_spawnattr_setsigmask(&posix_attr, &sigset_mask)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setsigmask: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}

	for (i = 2; i--; )
	{
		ret_err = posix_spawn( &pid_child[i_child++],
				argv_child_x[0],
				NULL,
				&posix_attr,
				argv_child_x,
				NULL);
	}


	/*2nd group */
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGUSR1);
	if ((ret_err = posix_spawnattr_setsigmask(&posix_attr, &sigset_mask)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, "Fail: attr_setsigmask: %s\n", buf_err);
		exit(EXIT_FAILURE);
	}
	
	for (i = 2; i--; )
	{
		ret_err = posix_spawn( &pid_child[i_child++],
				argv_child_y[0],
				NULL,
				&posix_attr,
				argv_child_y,
				NULL);
	}

	sleep(1);	/* wait stdio buffering */
	printf("Parent[%d]: # of Child processes [%d] \n", getpid(), i_child);
	for (i = 0; i < i_child; i++)
		printf("\t* Child[%d] : %d\n", i, pid_child[i]);
	kill(-getpgid(0), SIGUSR1); /* equivalent to killpg(getpgid(0), SIGUSR1) */
	kill(-getpgid(0), SIGUSR2);
	sleep(1);	/* wait stdio buffering */
	kill(-getpgid(0), SIGTERM);

	if ((ret_err = posix_spawnattr_destroy(&posix_attr)) != 0)
	{
		strerror_r(ret_err, buf_err, sizeof(buf_err));
		fprintf(stderr, buf_err, sizeof(buf_err));
		exit(EXIT_FAILURE);
	}

	printf("Parent[%d]: Wait for child\n", getpid());
	for (i = 0; i < i_child; i++)
		(void)waitpid(-1, NULL, WNOHANG);	/* wait for child */
	printf("Parent[%d]: Exit \n", getpid());
	sleep(1); /* wait for flushing child's stdio buf */
	return 0;
}
