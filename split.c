#include <stdio.h>
#include <malloc.h>
#include <string.h>

char* GiveMemoryForString(int len)
{
	char* str = (char*)malloc(len*sizeof(char));
	return str;
}

char** GiveMemoryForArray(int count, int len)
{
	char** array = (char**)malloc(count*sizeof(char*));
	int i;
	for(i = 0; i < count; i++)
	{
		array[i] = (char*)malloc(len*sizeof(char));
	}
	return array;
}

void FreeArrayMemory(char** array, int count)
{
	int i;
	for(i = 0; i < count; i++)
	{
		free(array[i]);
	}
	free(array);
}

void Split(char* string, char* delimiter, char** words, int* count)
{
	int i = 0;
	char* tmp = GiveMemoryForString(30);
	tmp = strtok(string, delimiter);
	while(tmp != NULL)
	{
		printf("%s\n", tmp);
		strcpy(words[i], tmp);
		i++;
		tmp = strtok(NULL, delimiter);
		*(count) = i;
	}
	free(tmp);
	
}

int main()
{
	char* string = GiveMemoryForString(100);
	fgets(string, 100, stdin);
	char* delimiter = GiveMemoryForString(10);
	fgets(delimiter, 10, stdin);
	int init_count = 15;
	char**words = GiveMemoryForArray(init_count, 15);
	int count = 0;
	
	Split(string, delimiter, words, &count);
	
	free(string);
	free(delimiter);
	FreeArrayMemory(words, init_count);

	return 0;
}
