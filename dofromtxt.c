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

Command* ReadTxt(int* commandCount, int* timeout);
void ExecuteCommands(Command* commands, int commandCount, int timeout);

int main()
{
	int commandCount = 0;
	int timeout = 0;
	Command* commands = ReadTxt(&commandCount, &timeout);
	ExecuteCommands(commands, commandCount, timeout);

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

Command*  ReadTxt(int* commandCount, int* timeout)
{
	FILE* commandFile = fopen("command.txt", "r");
	if(commandFile == NULL)
	{
		printf("Error with file\n");
		return 0;
	}

	char* str = (char*)malloc(STRING_LEN * sizeof(char));
	int i, j;
	int wordCounter;
	char** words = (char**)malloc(ARG_QUANTITY * sizeof(char*));
	for(i = 0; i < ARG_QUANTITY; i++)
	{
		words[i] = (char*)malloc(STRING_LEN * sizeof(char));
	}
	fgets(str, STRING_LEN, commandFile);
	ParseHeaderLine(str, words, wordCounter, commandCounter, timeout);
	Command* commands = (Command*)malloc(*commandCount * sizeof(Command));
	for(i = 0; i < *commandCount; i++)
	{
		fgets(str, STRING_LEN, commandFile);
		ParseCommandLine(commands, str, &wordCounter);
	}
	fclose(commandFile);
	free(str);
	for(i = 0; i < ARG_QUANTITY; i++)
	{
		free(words[i]);
	}
	free(words);
	return commands;
}

void ParseHeaderLine(char* str, char** words, int* wordCounter, int* commandCount, int* timeout)
{
	Split(str, " ", words, wordCounter);
	*commandCount = atoi(words[0]);
	*timeout = atoi(words[1]);
}

void ParseCommandLine(Command* commands, char* str, int* wordCounter)
{
	commands[i].name = (char*)malloc(STRING_LEN * sizeof(char));
	commands[i].arguments = (char**)malloc(ARG_QUANTITY * sizeof(char*));
	for(j = 0; j < ARG_QUANTITY; j++)
	{
		commands[i].arguments[j] = (char*)malloc(STRING_LEN * sizeof(char));
	}
	Split(str, " ", commands[i].arguments, wordCounter);
	commands[i].arguments[*wordCounter - 1][strlen(commands[i].arguments[*wordCounter - 1]) - 1] = '\0';
	commands[i].arguments[*wordCounter] = NULL;
	commands[i].time = atoi(commands[i].arguments[0]);
	commands[i].name = commands[i].arguments[1];
}

void ExecuteCommands(Command* commands, int commandCount, int timeout)
{
	int externalProcessId, childProcessId;
	int status;
	int i;
	int procid;
	for(i = 0; i < commandCount; i++)
	{
		externalProcessId = fork();
		if(externalProcessId == 0)
		{
			sleep(commands[i].time);
			childProcessId = fork();
			if(childProcessId == 0)
			{
				execvp(commands[i].name, (commands[i].arguments + 1));
			}
			else 
			{
				sleep(timeout);
				if(waitpid(childProcessId, &status, WNOHANG))
				{
					exit(0);
				}
				else
				{
					kill(childProcessId, SIGKILL);
					printf("Process %d killed\n", childProcessId);
					exit(0);
				}
			}
		}
	}
}

