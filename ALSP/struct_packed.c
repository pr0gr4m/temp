#include <stdio.h>
#include <stddef.h>

#define pr_struct(pfix_name)	printf("sizeof(MY_ST_"#pfix_name")=%d ", \
		sizeof(MY_ST_##pfix_name)); \
		printf("offsetof(...,cnt)=0x%04x\n", offsetof(MY_ST_##pfix_name,cnt))

typedef struct my_st_a {
	char	str[7];
	char	cnt[4];
} MY_ST_A;

typedef struct my_st_b {
	char	str[7];
	int		cnt;
} __attribute__((packed)) MY_ST_B;

int main()
{
	pr_struct(A);
	pr_struct(B);
	return 0;
}
