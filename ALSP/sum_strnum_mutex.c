#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "stdalsp.h"

#define NUM_THREADS	3
#define LEN_SUM_STR	16

struct thread_arg
{
	pthread_t	tid;
	int			idx;
	char		*x, *y;
} t_arg[NUM_THREADS] = {
	{ 0, 0, "1", "3" },
	{ 0, 1, "4", "4" },
	{ 0, 2, "1", "5" }
};

void *start_func(void *);
void clean_thread(struct thread_arg *);
char *sum_strnum(const char *, const char *);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	while (1) {
	for (int i = 0; i < 3; i++) {
		if (pthread_create(&t_arg[i].tid, NULL, start_func, &t_arg[i]) != 0) {
			exit(1);
		}
	}
	clean_thread(t_arg);
	}
	return EXIT_SUCCESS;
}

void *start_func(void *arg)
{
	struct thread_arg	*t_arg = (struct thread_arg *)arg;
	char	*ret_str = sum_strnum(t_arg->x, t_arg->y);
	if (t_arg->idx == 0)
		usleep(500000);
	printf("%s + %s = %s (%p) \n", t_arg->x, t_arg->y, ret_str, ret_str);
	pthread_exit(t_arg);
}

char *sum_strnum(const char *s1, const char *s2) {
	static char	buf_sum[LEN_SUM_STR];
	pthread_mutex_lock(&mutex);
	snprintf(buf_sum, sizeof(buf_sum), "%d", atoi(s1) + atoi(s2));
	pthread_mutex_unlock(&mutex);
	return buf_sum;
}

void clean_thread(struct thread_arg *p_arg)
{
	int		i;
	struct thread_arg	*t_arg_ret;
	for (i = 0; i < NUM_THREADS; i++, p_arg++) {
		pthread_join(p_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread join : %d - %lu", p_arg->idx, p_arg->tid);
	}
}
