#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <unistd.h>
#include "stdalsp.h"

#define LEN_SUM_STR	16

char *sum_strnum(const char *, const char *);

int main()
{
	omp_set_num_threads(4);
	{
#pragma omp parallel sections
	{
#pragma omp section
		{
			char *x = "1", *y = "3";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)",
					omp_get_thread_num(), x, y, ret_str, ret_str);
			sleep(1);
		}
#pragma omp section
		{
			char *x = "4", *y = "4";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)",
					omp_get_thread_num(), x, y, ret_str, ret_str);
			sleep(1);
		}
#pragma omp section
		{
			char *x = "1", *y = "5";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)",
					omp_get_thread_num(), x, y, ret_str, ret_str);
			sleep(1);
		}
	}
	}
	return 0;
}

char *sum_strnum(const char *s1, const char *s2)
{
	static char tls_str[LEN_SUM_STR];
#pragma omp threadprivate(tls_str)
	snprintf(tls_str, LEN_SUM_STR, "%d", atoi(s1) + atoi(s2));
	return tls_str;
}
