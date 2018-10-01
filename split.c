#include <stdio.h>
#include <malloc.h>
#include <string.h>


#define STRING_LEN 100
#define INIT_COUNT 15

char* GiveMemoryForString(int len)
{
	char* str = (char*)malloc(len * sizeof(char));
	return str;
}

char** GiveMemoryForArray(int count, int len)
{
	char** array = (char**)malloc(count * sizeof(char*));
	if(array == NULL)
	{
		printf("Error at allocation memory for array!\n");
		return 0;
	}
	else
	{
		int i;
		for(i = 0; i < count; i++)
		{
			array[i] = (char*)malloc(len * sizeof(char));
		}
		return array;
	}
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
	char* tmp = GiveMemoryForString(STRING_LEN);
	tmp = strtok(string, delimiter);
	while(tmp != NULL)
	{
		strcpy(words[i], tmp);
		i++;
		tmp = strtok(NULL, delimiter);
	}
	*count = i;
	free(tmp);
	
}

int main()
{
	char* string = GiveMemoryForString(STRING_LEN);
	fgets(string, STRING_LEN, stdin);
	char* delimiter = GiveMemoryForString(STRING_LEN);
	fgets(delimiter, STRING_LEN, stdin);
	char**words = GiveMemoryForArray(INIT_COUNT, STRING_LEN);
	int count = 0;
	
	Split(string, delimiter, words, &count);
	int i;
	for(i = 0; i < count; i++)
	{
		printf("%s\n", words[i]);
	}
	free(string);
	free(delimiter);
	FreeArrayMemory(words, init_count);

	return 0;
}
