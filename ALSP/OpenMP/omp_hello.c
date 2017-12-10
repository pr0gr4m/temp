#include <stdio.h>
#include <omp.h>

int main()
{
	omp_set_num_threads(2);
#pragma omp parallel
	{
		printf("Hello OpenMP\n");
	}
	return 0;
}
