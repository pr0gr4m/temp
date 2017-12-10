#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp_popen;
	int n_read;
	char a_buf[1024];
	if ((fp_popen = popen("ls -l", "r")) == NULL) {
		perror("popen error");
		exit(EXIT_FAILURE);
	}

	while (!feof(fp_popen)) {
		if ((n_read = fread(a_buf, sizeof(char), sizeof(a_buf), fp_popen)) == -1) {
			perror("fread error");
			exit(EXIT_FAILURE);
		}

		if (n_read == 0)
			break;
		printf("[%1$d byte] %2$.*1$s\n", (int)n_read, a_buf);
	}

	pclose(fp_popen);
	return 0;
}
