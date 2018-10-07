#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>

#define STRING_LEN 50
#define ARG_QUANTITY 10

struct Command
{
	int time;
	char* name;
	char** arguments;
} typedef Command;

// Может commandCount? Также вместо comarray можно написать commands просто.
// Вы Split не используете явно в main. Можно убрать объявление совсем, а оставить только определение после main.

void Split(char* string, char* delimiter, char** words, int* count);
Command* ReadTxt(int* comcounter, int* timeout);
void ExecuteCommands(Command* comarray, int comcounter, int timeout);

int main()
{
	int comcounter = 0;
	int timeout = 0;
	Command* comarray = ReadTxt(&comcounter, &timeout);
	ExecuteCommands(comarray, comcounter, timeout);

	return 0;
}

void Split(char* string, char* delimiter, char** words, int* count)
{
	int i = 0;
	char* tmp = (char*)malloc(STRING_LEN * sizeof(char));
   	tmp = strtok(string, delimiter);
        while(tmp != NULL)
        {
            strcpy(words[i], tmp);
            i++;
            tmp = strtok(NULL, delimiter);
        }
	*(count) = i;
	free(tmp);
}

Command*  ReadTxt(int* comcounter, int* timeout)
{
	FILE* comfile = fopen("command.txt", "r");
	if(comfile == NULL)
	{
		printf("Error with file\n");
		return 0;
	}

	char* str = (char*)malloc(STRING_LEN * sizeof(char));
	int i, j;
	int wordcounter;
	char** words = (char**)malloc(ARG_QUANTITY * sizeof(char*));
	for(i = 0; i < ARG_QUANTITY; i++)
	{
		words[i] = (char*)malloc(STRING_LEN * sizeof(char));
	}
	fgets(str, STRING_LEN, comfile);
	// У вас эта ф-я вышла достаточно громоздкой. Лучше бы вынести её куски в отдельные ф-и, например,
	// три строки ниже тянут на ParseHeaderLine
	Split(str, " ", words, &wordcounter);
	*comcounter = atoi(words[0]);
	*timeout = atoi(words[1]);
	Command* comarray = (Command*)malloc(*comcounter * sizeof(Command));
	for(i = 0; i < *comcounter; i++)
	{
		fgets(str, STRING_LEN, comfile);
		// А весь кусок ниже на ParseCommandLine
		comarray[i].name = (char*)malloc(STRING_LEN * sizeof(char));
		comarray[i].arguments = (char**)malloc(ARG_QUANTITY * sizeof(char*));
		for(j = 0; j < ARG_QUANTITY; j++)
		{
			comarray[i].arguments[j] = (char*)malloc(STRING_LEN * sizeof(char));
		}
		Split(str, " ", comarray[i].arguments, &wordcounter);
		comarray[i].arguments[wordcounter - 1][strlen(comarray[i].arguments[wordcounter - 1]) - 1] = '\0';
		comarray[i].arguments[wordcounter] = NULL;
		comarray[i].time = atoi(comarray[i].arguments[0]);
		comarray[i].name = comarray[i].arguments[1];
	}
	fclose(comfile);
	free(str);
	for(i = 0; i < ARG_QUANTITY; i++)
	{
		free(words[i]);
	}
	free(words);
	return comarray;
}

// Мы договаривались про именовании переменных, что первое слово со строчной, остальные с заглавной: commandCount
void ExecuteCommands(Command* comarray, int comcounter, int timeout)
{
	// FIXIT: pid1 и pid2 нужно переименовать в нечто более говорящее о предназначении переменной
	int pid1, pid2;
	int status;
	int i;
	int procid;
	for(i = 0; i < comcounter; i++)
	{
		pid1 = fork();
		if(pid1 == 0)
		{
			sleep(comarray[i].time);
			pid2 = fork();
			if(pid2 == 0)
			{
				execvp(comarray[i].name, (comarray[i].arguments + 1));
			}
			else 
			{
				sleep(timeout);
				if(waitpid(pid2, &status, WNOHANG))
				{
					exit(0);
				}
				else
				{
					kill(pid2, SIGKILL);
					printf("Process %d killed\n", pid2);
					exit(0);
				}
			}
		}
	}
}

