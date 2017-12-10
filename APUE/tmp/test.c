#include <stdio.h>

void my_print(char *);
int my_sum(int, int);

int main(void)
{
	my_print("Hello world \n");
	printf("3 + 4 = %d \n", my_sum(3, 4));
	return 0;
}
