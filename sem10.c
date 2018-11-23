#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define STRING_LEN 100

void find(char* cur_dir, int cur_depth, int max_depth,char* search_name)
{
	if(cur_depth > max_depth)
	{
		return;
	}
	DIR* curdir = opendir(cur_dir);
	struct dirent* entry;
	entry = readdir(curdir);
	struct stat statbuf;
	
	while(entry)
	{
		if((strcmp(entry->d_name, ".") == 0) ||
			       	(strcmp(entry->d_name, "..") == 0))
		{
		}
		else
		{
			char* strcopy = (char*)malloc(STRING_LEN * sizeof(char));
			strcat(strcopy, cur_dir);
			strcat(strcopy, "/");
			strcat(strcopy, entry->d_name);

			stat(strcopy, &statbuf);
			if(S_ISREG(statbuf.st_mode))
			{
				if(strcmp(entry->d_name, search_name) == 0)
				{
					printf("Found at %s/\n", cur_dir);
					return;
				}
			}
			if(S_ISDIR(statbuf.st_mode))
			{
				find(strcopy, cur_depth + 1, max_depth, search_name);
			}
		}
		entry = readdir(curdir);	
	}
	return;

}

int main(int argc, char* argv[])
{
	int maxdepth = atoi(argv[2]);
	char* filename = (char*)malloc(STRING_LEN * sizeof(char));
	strcat(filename, argv[3]);
	find(argv[1], 0, maxdepth, filename);
	free(filename);
	return 0;
}

