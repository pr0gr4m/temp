#include <stdio.h>
#include <omp.h>

int main()
{
	int i, j;
	omp_set_num_threads(4);
	omp_set_nested(1);
#pragma omp parallel for
	for (i = 9; i > 6; i--) {
#pragma omp parallel for
		for (j = 0; j < 5; j++) {
			printf("[%d] (i,j=%d,%d)\n", omp_get_thread_num(), i, j);
		}
	}

	return 0;
}
