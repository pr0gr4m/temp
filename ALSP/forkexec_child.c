#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

int main()
{
	dprintf(STDOUT_FILENO, "Child[%d]: Start\n", getpid());
	dprintf(3, "child[%d]: fd(3): Test fd.\n", getpid());
	close(3);
	dprintf(STDOUT_FILENO, "Child[%d]: Exit\n", getpid());
	return 0;
}
