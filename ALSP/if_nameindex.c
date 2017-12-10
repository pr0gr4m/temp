#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>

int main()
{
	int		i;
	struct if_nameindex	*ifnames;
	 if ((ifnames = if_nameindex()) == NULL) {
		 perror("Fail: if_nameindex");
		 exit(EXIT_FAILURE);
	 }

	 for (i = 0; ifnames[i].if_index != 0 && ifnames[i].if_name != NULL; i++) {
		 printf("if_nameindex[%d] : if_index(%d) if_name(%s)\n", i,
				 ifnames[i].if_index, ifnames[i].if_name);
	 }

	 if_freenameindex(ifnames);
	 return 0;
}
