#include <stdio.h>

int checkFile(char *path)
{
	FILE *found;
	found = fopen(path, "r");
	if (found == NULL){
		return 0;
	}
	fclose(found);
	return 1;
}
