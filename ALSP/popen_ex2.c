#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp_popen;
	int i, rc_write;
	size_t len_buf = 0;
	ssize_t rc_getline = 0;
	char *p_buf;

	if ((fp_popen = popen("sort", "w")) == NULL) {
		perror("popen error");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 5; i++) {
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) {
			perror("getline error");
			exit(EXIT_FAILURE);
		}
		if ((rc_write = fwrite(p_buf, sizeof(char), rc_getline, fp_popen)) == -1) {
			perror("fwrite error");
			exit(EXIT_FAILURE);
		}

		if (rc_write == 0)
			break;

		free(p_buf);
		p_buf = NULL;
	}

	printf("* Sorting data -> \n");
	pclose(fp_popen);
	return 0;
}
