#include <stdio.h>
#include <omp.h>

int main()
{
	int i, j;
#pragma omp parallel num_threads(4)
	{
#pragma omp for private(j)
		for (i = 9; i > 6; i--) {
			printf("[%d] (i = %d) \n", omp_get_thread_num(), i);
			for (j = 0; j < 5; j++) {
				printf("[%d] (i , j = %d, %d)\n", omp_get_thread_num(), i, j);
			}
		}
	}

	return 0;
}
