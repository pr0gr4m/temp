#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define GET_TIME0(a)	get_time0(a, sizeof(a)) == NULL ? "error" : a

char *get_time0(char *buf, size_t sz_buf);
int inst_timer(void);
void sa_sigaction_rtmin(int signum, siginfo_t *si, void *sv);

int main()
{
	if (inst_timer() == -1) {
		return EXIT_FAILURE;
	}
	while (1) {
		pause();
	}

	return EXIT_SUCCESS;
}

int inst_timer(void)
{
	struct sigaction sa_rt1;
	struct sigevent sigev;
	timer_t rt_timer;
	struct itimerspec rt_itspec;
	struct timespec tspec;
	char ts_now[20];

	memset(&sa_rt1, 0, sizeof(sa_rt1));
	sigemptyset(&sa_rt1.sa_mask);
	sa_rt1.sa_sigaction = sa_sigaction_rtmin;
	sa_rt1.sa_flags = SA_SIGINFO;

	if (sigaction(SIGRTMIN, &sa_rt1, NULL) == -1) {
		perror("FAIL: sigaction()");
		return -1;
	}

	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGRTMIN;

	if (timer_create(CLOCK_MONOTONIC, &sigev, &rt_timer) == -1) {
		perror("FAIL: timer_create()");
		return -1;
	}

	clock_gettime(CLOCK_MONOTONIC, &tspec);
	tspec.tv_sec += 2;
	tspec.tv_nsec = 500000000;
	if (tspec.tv_nsec > 999999999) {
		tspec.tv_sec += 1;
		tspec.tv_nsec -= 1000000000;
	}
	
	rt_itspec.it_value = tspec;
	rt_itspec.it_interval.tv_sec = 4;
	rt_itspec.it_interval.tv_nsec = 0;
	printf("Enable timer at %s.\n", GET_TIME0(ts_now));
	if (timer_settime(rt_timer, TIMER_ABSTIME, &rt_itspec, NULL) == -1) {
		perror("FAIL: timer_settime()");
		return -1;
	}

	return 0;
}

void sa_sigaction_rtmin(int signum, siginfo_t *si, void *sv)
{
	char ts_now[20];
	printf("-> RT Timer expiration at %s\n", GET_TIME0(ts_now));
}

#define STR_TIME_FORMAT	"%H:%M:%S"

char *get_time0(char *buf, size_t sz_buf)
{
	struct timespec tspec;
	struct tm tm_now;
	if (buf == NULL)
		return NULL;
	if (clock_gettime(CLOCK_REALTIME, &tspec) == -1) {
		return NULL;
	}

	localtime_r((time_t *)&tspec.tv_sec, &tm_now);
	if (strftime(buf, sz_buf, STR_TIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}

	return buf;
}
