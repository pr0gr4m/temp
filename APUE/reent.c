#include "apue.h"
#include <pwd.h>

static void
my_alarm(int sgno)
{
	struct passwd *rootptr;

	printf("in signal handler\n");
	if ((rootptr = getpwnam("root")) == NULL)
		err_sys("getpwnam(root) error");
	alarm(1);
}

int
main(void)
{
	struct passwd *ptr;

	signal(SIGALRM, my_alarm);
	alarm(1);

	for ( ; ; )
	{
		if ((ptr = getpwnam("pr0gr4m")) == NULL)
			err_sys("getpwnam error");
		if (strcmp(ptr->pw_name, "pr0gr4m") != 0)
			printf("return value corrumpted!, pw_name = %s\n",
					ptr->pw_name);
	}
}
