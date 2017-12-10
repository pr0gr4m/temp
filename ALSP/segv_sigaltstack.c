#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int exhaust_stack(int count);
void inst_sighandler();
void sa_handler_segv(int signum);

int flag_altstack;
#define SZ_SIGHANDLER_STACK	16384
#define SZ_BUFFER	1000000

stack_t g_ss;

int main(int argc, char *argv[])
{
	if (argc == 2 && argv[1][0] == '1') {
		printf("[enabled] alternate signal stack.\n");
		flag_altstack = 1;
	} else {
		printf("[disabled] alternate signal stack.\n");
	}

	printf("SIGSTKSZ(%d) MINSIGSTKSZ(%d)\n", SIGSTKSZ, MINSIGSTKSZ);
	inst_sighandler();
	exhaust_stack(100);
	return 0;
}

int exhaust_stack(int count)
{
	char buffer[SZ_BUFFER] = { 1, };

	if (count <= 0) {
		printf(">> stopping recursive func.\n");
		return 0;
	}

	printf("[%d] Current stack addr(%p) \n", count, buffer);
	exhaust_stack(count - 1);
	return 0;
}

void sa_handler_segv(int signum)
{
	time_t t_now = time(0);
	struct tm *tm_now = localtime(&t_now);
	snprintf(g_ss.ss_sp, g_ss.ss_size,
			"SEGV: SigNo(%d) Time(%02d:%02d:%02d)",
			signum,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
	printf("%s\n", (char *)g_ss.ss_sp);
	fflush(stdout);
}

void inst_sighandler()
{
	struct sigaction sa_segv;
	memset(&sa_segv, 0, sizeof(struct sigaction));
	if ((g_ss.ss_sp = malloc(SZ_SIGHANDLER_STACK)) == NULL) {
		exit(EXIT_FAILURE);
	}

	g_ss.ss_size = SZ_SIGHANDLER_STACK;
	g_ss.ss_flags = 0;
	if (flag_altstack) {
		if (sigaltstack(&g_ss, NULL) == -1) {
			exit(EXIT_FAILURE);
		}
		sa_segv.sa_flags = SA_ONSTACK | SA_RESETHAND;
	} else {
		sa_segv.sa_flags = 0;
	}

	sa_segv.sa_handler = sa_handler_segv;
	sigaction(SIGSEGV, &sa_segv, 0);
}
