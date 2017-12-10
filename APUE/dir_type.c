#include "apue.h"
#include <dirent.h>
#include <fcntl.h>
int
main(int argc, char *argv[])
{
	int i;
	struct stat buf;
	char *ptr;
	DIR *dp;
	struct dirent *dirp;

	close(open("test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644));

	if ((dp = opendir(".")) == NULL)
		err_sys("can't open %s", ".");

	while ((dirp = readdir(dp)) != NULL)
	{
		printf("%s: ", dirp->d_name);
		if (lstat(dirp->d_name, &buf) < 0)
		{
			err_ret("lstat error");
			continue;
		}
		if (S_ISREG(buf.st_mode))
			ptr = "regular";
		else if (S_ISDIR(buf.st_mode))
			ptr = "directory";
		else if (S_ISCHR(buf.st_mode))
			ptr = "character special";
		else if (S_ISBLK(buf.st_mode))
			ptr = "block special";
		else if (S_ISFIFO(buf.st_mode))
			ptr = "fifo";
		else if (S_ISLNK(buf.st_mode))
			ptr = "symbolic link";
		else if (S_ISSOCK(buf.st_mode))
			ptr = "socket";
		else
			ptr = "** unknown mode **";
		printf("%s\n", ptr);
	}
	exit(0);
}
