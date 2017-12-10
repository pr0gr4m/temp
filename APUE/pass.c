#include "apue.h"

char *getpass(const char *);

int
main(void)
{
	char *ptr;

	if ((ptr = getpass("Enter password:")) == NULL)
		err_sys("getpass error");
	printf("password: %s\n", ptr);

	while (*ptr != 0)
		*ptr++ = 0;
	exit(0);
}
