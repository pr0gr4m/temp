#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "stdalsp.h"

#define NUM_THREADS	5
#define MAX_ITEMS	64
#define PATH_FIFO	"/tmp/my_fifo"

struct workqueue
{
	int		item[MAX_ITEMS];
	int		idx;
	int		cnt;
	pthread_mutex_t	mutex;
	pthread_cond_t	cv;
} *wq;

void *tfunc_a(void *);
void *tfunc_b(void *);

struct thread_arg
{
	pthread_t	tid;
	int			idx;
	void *(* func)(void *);
} t_arg[] = {
	{ 0, 0, tfunc_a },
	{ 0, 0, tfunc_b },
	{ 0, 0, tfunc_b },
	{ 0, 0, tfunc_b },
	{ 0, 0, NULL }
};

int push_item(struct workqueue *wq, const char *item, int cnt);
int pop_item(struct workqueue *wq, int *item);
int process_job(int *);
void clean_thread(struct thread_arg *);

int main()
{
	int		i;
	if ((wq = calloc(1, sizeof(struct workqueue))) == NULL) {
		pr_err("calloc(%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}
	pthread_mutex_init(&wq->mutex, NULL);
	pthread_cond_init(&wq->cv, NULL);
	for (i = 0; i < NUM_THREADS && t_arg[i].func != NULL; i++) {
		t_arg[i].idx = i;
		if (pthread_create(&t_arg[i].tid, NULL, t_arg[i].func, (void *)&t_arg[i])) {
			return EXIT_FAILURE;
		}
		pr_out("pthread_create : tid = %lu", t_arg[i].tid);
	}
	clean_thread(t_arg);
	return EXIT_SUCCESS;
}

void *tfunc_a(void *arg)
{
	int		fd, ret_read = 0;
	char	buf[MAX_ITEMS / 2];
	pr_out(" >> Thread (A) Started!");
	if (mkfifo(PATH_FIFO, 0644) == -1) {
		if (errno != EEXIST) {
			pr_err("[A] FAIL: mkfifo : %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if ((fd = open(PATH_FIFO, O_RDONLY, 0644)) == -1) {
		pr_err("[A] FAIL: open : %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((ret_read = read(fd, buf, sizeof(buf))) == -1) {
			pr_err("[A] FAIL: read : %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (ret_read == 0) {
			pr_err("[A] broken pipe: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		push_item(wq, buf, ret_read);
		pr_out("[A] cond_signal");
		pthread_cond_broadcast(&wq->cv);
	}
	return NULL;
}

void *tfunc_b(void *arg)
{
	int		item;
	pr_out(" >> Thread (B) started!");
	while (1) {
		pop_item(wq, &item);
		process_job(&item);
	}
	return NULL;
}

int push_item(struct workqueue *wq, const char *item, int cnt)
{
	int i, j;
	pthread_mutex_lock(&wq->mutex);
	for (i = 0, j = (wq->idx + wq->cnt) % MAX_ITEMS; i < cnt; i++, j++, wq->cnt++) {
		if (wq->cnt == MAX_ITEMS) {
			pr_err("[Q:%d,%d] queue full : wq(idx,cnt=%d,%d)",
					i, j, wq->idx, wq->cnt);
			break;
		}
		if (j == MAX_ITEMS)
			j = 0;
		wq->item[j] = (int) item[i];
		pr_out("[Q:%d,%d] push(idx,cnt=%d,%d) : item=(%c)", i, j, wq->idx, wq->cnt, item[i]);
	}
	pthread_mutex_unlock(&wq->mutex);
	return i;
}

int pop_item(struct workqueue *wq, int *item)
{
	pthread_mutex_lock(&wq->mutex);
	while (1) {
		if (wq->cnt > 0) {
			if (wq->idx == MAX_ITEMS)
				wq->idx = 0;
			*item = wq->item[wq->idx];
			wq->idx++;
			wq->cnt--;
			pr_out("[B] pop(%d,%d) item(%c) (tid=%ld)",
					wq->idx, wq->cnt, (char)*item, pthread_self());
			break;
		} else {
			pr_out("[B] cond_wait (tid=%ld)", pthread_self());
			pthread_cond_wait(&wq->cv, &wq->mutex);
			pr_out("[B] Wake up (tid=%ld)", pthread_self());
		}
	}
	pthread_mutex_unlock(&wq->mutex);
	return 0;
}

int process_job(int *item)
{
	pr_out("[B] item=%d", *item);
	sleep(*item % 5 + 1);
	return 0;
}

void clean_thread(struct thread_arg *t_arg)
{
	int		i;
	struct thread_arg	*t_arg_ret;
	for (i = 0; i < NUM_THREADS; i++, t_arg++) {
		pthread_join(t_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread_join : %d - %lu", t_arg->idx, t_arg->tid);
	}
}
