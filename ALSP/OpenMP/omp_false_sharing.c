#include <stdio.h>
#include <omp.h>

#define NUM_THREADS	4
#define ITER_LOOP	400000000

int cnt_sheep[NUM_THREADS];
int count_sheep(int);

int main()
{
	int i;
#ifdef _OPENMP
	omp_set_num_threads(NUM_THREADS);
#endif
//#pragma omp parallel for
	for (i = 0; i < NUM_THREADS; i++) {
		count_sheep(i);
	}

	return 0;
}

int count_sheep(int idx)
{
	int i;
	for (i = idx; i < ITER_LOOP; i++) {
		cnt_sheep[idx] += (i % 2);
	}

	printf("[idx : %d] sum (%d) \n", idx, cnt_sheep[idx]);
	return 0;
}
