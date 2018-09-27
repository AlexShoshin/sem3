#include <stdio.h>
#include<malloc.h>
#include<string.h>

void GenerateString(int n, char* string)
{
	char* tmp = (char*)malloc((1 << n) * sizeof(char));
	string[0] = 'a';
	string[1] = '\0';
	char letter = 'a';
	int i;
	for(i = 1; i < n; i++)
	{
		strcpy(tmp, string);
		letter += 1;
		string[(1 << i) - 1] = letter;
		string[(1 << i)] = '\0';
		strcat(string, tmp);
	}
	free(tmp);
}

int main()
{
	int n;
	scanf("%d", &n);
	char* string = (char*)malloc((1<<n)*sizeof(char));
	GenerateString(n, string);
	printf("%s\n", string);
	free(string);
	return 0;
}
