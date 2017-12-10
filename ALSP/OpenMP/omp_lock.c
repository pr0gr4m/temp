#include <stdio.h>
#include <unistd.h>
#include <omp.h>

omp_lock_t	mylock;

int main()
{
	omp_init_lock(&mylock);
#pragma omp parallel num_threads(4)
	{
#pragma omp sections
		{
#pragma omp section
			{
				omp_set_lock(&mylock);
				sleep(1);
				printf("[%d] 1. Hello world\n", omp_get_thread_num());
				omp_unset_lock(&mylock);
			}
#pragma omp section
			{
				omp_set_lock(&mylock);
				sleep(1);
				printf("[%d] 2. Hello world\n", omp_get_thread_num());
				omp_unset_lock(&mylock);
			}
#pragma omp section
			{
				omp_set_lock(&mylock);
				sleep(1);
				printf("[%d] 3. Hello world\n", omp_get_thread_num());
				omp_unset_lock(&mylock);
			}
#pragma omp section
			{
				omp_set_lock(&mylock);
				sleep(1);
				printf("[%d] 4. Hello world\n", omp_get_thread_num());
				omp_unset_lock(&mylock);
			}
		} /* sections */
	} /* parallel */
	omp_destroy_lock(&mylock);
	return 0;
}
