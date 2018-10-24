#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#define STR_LEN 100

int main(int argc, char* argv[])
{
	int size;
	char name0[] = "aaa.fifo";
	char name1[] = "aab.fifo";
	mkfifo(name0, 0777);
	mkfifo(name1, 0777);
	
	char* strFromFifo = (char*)malloc(STR_LEN * sizeof(char));
	char* strFromUser = (char*)malloc(STR_LEN * sizeof(char));
	int procId = fork();
	if(procId < 0)
	{
		printf("Can't create fork!\n");
		exit(0);
	}
	else if(procId == 0)
	{
		int fdRead;
		
		// Кажется можно ещё подсократить код так
		// fdRead = open(strcmp(argv[1], "0") == 0 ? name0 : name1, O_RDONLY);
		// fdWrite = open(strcmp(argv[1], "1") == 0 ? name1 : name0, O_RDONLY);
		
		if(strcmp(argv[1], "0") == 0)
		{
			fdRead = open(name0, O_RDONLY);
		}
		else
		{
			fdRead = open(name1, O_RDONLY);
		}
		if(fdRead == 0)
		{
			printf("Can't open fifo to read!\n");
			exit(0);
		}

		while(1)
		{
			// мы обсуждали, что можно хорошо бы проверить, что вернул read, и если 0, то можно выйти из цикла
			read(fdRead, strFromFifo, STR_LEN);
			printf("%s", strFromFifo);
		}

	}
	else 
	{
		int fdWrite;
		if(strcmp(argv[1], "0") == 0)
		{
			fdWrite = open(name1, O_WRONLY);
			printf("0");
		}
		else
		{
			fdWrite = open(name0, O_WRONLY);
			
		}
		if(fdWrite == 0)
		{
			printf("Can't open fifo to write!\n");
			exit(0);
		}
		while(1)
		{
			fgets(strFromUser, STR_LEN, stdin);
			write(fdWrite, strFromUser, strlen(strFromUser));
		}

	}

	free(strFromUser);
	free(strFromFifo);

	return 0;
}
