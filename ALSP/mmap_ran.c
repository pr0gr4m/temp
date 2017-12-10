#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MMAP_FILENAME	"mmapfile.dat"
#define MMAP_SIZE		64

char contents[128];

int main(int argc, char *argv[])
{
	int		fd, flag_mmap = MAP_SHARED;
	char	*p_map, a_input[100];

	if ((fd = open(MMAP_FILENAME, O_RDWR | O_CREAT, 0664)) == -1) {
		printf("Fail: open(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if (ftruncate(fd, MMAP_SIZE) == -1) {
		printf("Fail: ftruncate(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if ((p_map = mmap((void *)0, MMAP_SIZE, PROT_READ | PROT_WRITE, flag_mmap, fd, 0))
			== MAP_FAILED) {
		printf("Fail: mmap(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	close(fd);
	printf("* mmap file : %s\n", MMAP_FILENAME);

	while (1) {
		printf("'*' print current mmap otherwise input text to mmap. >>");
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) {
			perror("fgets error");
			return EXIT_FAILURE;
		}
		if (a_input[0] == '*') {
			printf("Current mmap -> '%.*s'\n", MMAP_SIZE, p_map);
		} else {
			a_input[strlen(a_input) - 1] = 0;
			memcpy(p_map, a_input, strlen(a_input));
			if (msync(p_map, MMAP_SIZE, MS_SYNC) == -1) {
				printf("Fail: msync(): (%d:%s)\n", errno, strerror(errno));
				return EXIT_FAILURE;
			}
		}
	}
	return EXIT_SUCCESS;
}
