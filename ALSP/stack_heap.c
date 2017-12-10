#include <stdio.h>

#define SZ_BUFFER	1024 * 1024

int exhaust_stack(int count)
{
	char buffer[SZ_BUFFER];
	if (count <= 0) {
		printf(">> reach break position, stop recursive function!!\n");
		return 0;
	}

	sprintf(buffer, ">> exhaust 1MiB stack(addr:%p), will more %d MiB...\n", buffer, count - 1);
	printf(buffer);
	exhaust_stack(count - 1);
	return 0;
}

int main(void)
{
	exhaust_stack(10);
	return 0;
}
