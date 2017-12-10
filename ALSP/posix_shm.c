#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NAME_POSIX_SHM	"/mmapfile"
#define SZ_SHM_SEGMENT	4096

int main()
{
	int		shm_fd;
	char	*shm_ptr;
	int		n_read = 0;
	size_t	n_input = 128;
	char	*p_input = (char *)malloc(n_input);

	printf("* SHM Name : %s \n", NAME_POSIX_SHM);
	if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR | O_CREAT | O_EXCL, 0660)) > 0) {
		printf("* Create SHM : /dev/shm/%s\n", NAME_POSIX_SHM);
		if (ftruncate(shm_fd, SZ_SHM_SEGMENT) == -1)
			exit(EXIT_FAILURE);
	} else {
		if (errno != EEXIST) {
			exit(EXIT_FAILURE);
		}
		if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR, 0)) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	shm_ptr = (char *)mmap(NULL, SZ_SHM_SEGMENT, PROT_READ | PROT_WRITE,
			MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		exit(EXIT_FAILURE);
	}

	printf("'*' Print current shm.\n'.' Exit.\n");
	printf("otherwise change shm to your input.\n");

	while (1) {
		printf("\n>> ");
		if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) {
			perror("getline error");
			return -1;
		}

		if (p_input[0] == '.') {
			break;
		} else if (p_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			p_input[n_read - 1] = 0;
			memcpy(shm_ptr, p_input, n_read - 1);
		}
	}
	munmap(shm_ptr, SZ_SHM_SEGMENT);
	printf("* Would you remove shm (name : %s) (y/n) ", NAME_POSIX_SHM);

	if ( (n_read = (int)getline(&p_input, &n_input, stdin)) == -1) {
		perror("getline error");
		return -1;
	}

	if (p_input[0] == 'y') {
		shm_unlink(NAME_POSIX_SHM);
	}

	return 0;
}
