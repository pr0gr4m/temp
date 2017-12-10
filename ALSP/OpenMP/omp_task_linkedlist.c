#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define LIST_ITERATION	20

typedef struct lnklist	LNKLIST;

struct lnklist {
	int		num;
	LNKLIST	*next;
} *ll_head, *ll_tail;

void make_lnklist();
LNKLIST *append_lnklist(LNKLIST *l);
void walk_lnklist();

int main()
{
	make_lnklist();
#pragma omp parallel num_threads(4)
#pragma omp single
	{
		walk_lnklist();
	}
	return 0;
}

void walk_lnklist()
{
	int		i;
	LNKLIST	*list;
	for (i = 0, list = ll_head; list != NULL; i++, list = list->next) {
#pragma omp task firstprivate(i, list) if (i > 5)
		{
			printf("[%02d : %d] (%p -> %p)\n", i, omp_get_thread_num(), list, list->next);
			sleep(1);
		}
	}
}

void make_lnklist()
{
	int		i;
	LNKLIST	*list;

	ll_tail = ll_head = calloc(1, sizeof(LNKLIST));
	ll_head->next = ll_tail;
	for (i = 0; i < LIST_ITERATION; i++) {
		list = calloc(1, sizeof(LNKLIST));
		list->num = i;
		append_lnklist(list);
	}
}

LNKLIST *append_lnklist(LNKLIST *list)
{
	ll_tail->next = list;
	ll_tail = list;
	list->next = NULL;
	return list;
}
