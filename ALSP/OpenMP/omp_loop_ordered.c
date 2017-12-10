#include <stdio.h>
#include <omp.h>

int main()
{
	int i;
#pragma omp parallel num_threads(4)
#pragma omp for ordered
	for (i = 0; i < 32; i++) {
		printf("[%d] Hello OpenMP (%d)\n", i, omp_get_thread_num());
#pragma omp ordered
		{
			printf("\t[%d] Hello OpenMP : ordered block. (%d) \n", i, omp_get_thread_num());
		}
	}
	return 0;
}
