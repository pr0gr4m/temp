#include "apue.h"

int main(int argc, char *argv[])
{
	int i;
	int rnum = 0, dnum = 0, snum = 0, elnum = 0;

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "regular"))
			rnum++;
		else if (!strcmp(argv[i], "directory"))
			dnum++;
		else if (!strcmp(argv[i], "symbolic"))
			snum++;
		else
			elnum++;
	}
	printf("\n************ file type number *************\n");
	printf("[regular: %d]\n", rnum);
	printf("[directory: %d]\n", dnum);
	printf("[link: %d]\n", snum);
	printf("[else: %d]\n", elnum);
	
	return 0;
}
