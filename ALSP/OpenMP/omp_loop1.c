#include <stdio.h>
#include <omp.h>

int main()
{
	int i;
#pragma omp parallel num_threads(2)
#pragma omp for
	for (i = 0; i < 8; i++) {
		printf("[%d] Hello OpenMP (%d)\n", i, omp_get_thread_num());
	}

	return 0;
}
