#include <stdio.h>

int checkFile(char *path)
{
	FILE *found;
	found = fopen(path, "r");
	if (found == NULL){
		fclose(found);
		return 0;
	}
	fclose(found);
	return 1;
}
