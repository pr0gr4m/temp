#include <stdio.h>
#include <omp.h>

int num_steps = 400000000;

int main()
{
	int i;
	double x, step, sum = 0.0;
	step = 1.0 / (double)num_steps;
#pragma omp parallel num_threads(4)
#pragma omp for private(x) reduction(+:sum)
	for (i = 0; i < num_steps; i++)
	{
		x = (i + 0.5) * step;
		sum += 4.0 / (1.0 + x * x);
	}

	printf("pi = %.8f (sum = %.8f)\n", step * sum, sum);
	return 0;
}
