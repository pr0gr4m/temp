#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main()
{
	if (execl("/bin/ls", "ls", "-al", NULL) == -1)
	{
		perror("execl");
	}

	printf("+ after execl\n");
	return 0;
}
