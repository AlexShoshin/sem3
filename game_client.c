#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<signal.h>

#define SENDLINE_LEN 3
#define RECVLINE_LEN 3
#define MATRIX_SIZE 216
#define FULL_MATRIX 220
#define MATRIX_LENGTH 18
#define MATRIX_WIDTH 12

void Connect_to_game(char* sendline, char* recvline, int sockfd, struct sockaddr_in* servaddr, int* pl_number, int* pl_quantity);
void Send_movement_to_server(int sockfd, char* sendline, int* symbol_flag, int* my_number, struct sockaddr_in* servaddr, int pid);
void Change_statistics(int sockfd, char* sendline, char* recvline, int* my_number, int* life, struct sockaddr_in* servaddr);
void Print_game_field(char* recvline);


int main(int argc, char **argv)
{
        int sockfd;
        int n, len;
	char sendline[SENDLINE_LEN], recvline[RECVLINE_LEN];
        struct sockaddr_in servaddr, cliaddr;
        if(argc != 2)
	{
                exit(-1);
        }
        if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
                exit(-1);
        }
        bzero(&cliaddr, sizeof(cliaddr));
        cliaddr.sin_family = AF_INET;
        cliaddr.sin_port = htons(0);
        cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0)
	{
                perror(NULL);
                close(sockfd);
                exit(1);
        }
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(51069);
        if(inet_aton(argv[1], &servaddr.sin_addr) == 0)
	{
                close(sockfd);
                exit(1);
        }

	int i, j;
	int my_number;
	int players_quantity;

	Connect_to_game(sendline, recvline, sockfd, &servaddr, &my_number, &players_quantity);

	int life[4];
	for(i = 0; i < 4; i++)
	{
		life[i] = 100;
	}

	int pid = fork();
	int symbol_flag = 1;
	int die_flag = 0;
	if(pid == 0)
	{
		Send_movement_to_server(sockfd, sendline, &symbol_flag, &my_number, &servaddr, pid);
	}
	else
	{
		while(1)
		{
			recvfrom(sockfd, recvline, FULL_MATRIX, 0, (struct sockaddr *) NULL, NULL);
			Change_statistics(sockfd, sendline, recvline, &my_number, life, &servaddr);
			Print_game_field(recvline);	
		}
	}
	close(sockfd);
	return 0;
}





void Connect_to_game(char* sendline, char* recvline, int sockfd, struct sockaddr_in* servaddr, int* pl_number, int* pl_quantity)
{
	int i, j;
	char pl_choice;
	printf("Are you ready?(y)\n");
	scanf("%c", &pl_choice);
	while(pl_choice != 'y')
	{
		while(pl_choice != '\n')
		{
                	scanf("%c", &pl_choice);
		}
                printf("\r\e[A                           \r");
		scanf("%c", &pl_choice);
	}
	sendline[0] = pl_choice;
	sendline[1] = 'i'; //means incoming
	sendline[2] = '\0';
	sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) servaddr, sizeof(*servaddr));
	recvfrom(sockfd, recvline, RECVLINE_LEN, 0, (struct sockaddr *) NULL, NULL);
	int my_number = recvline[0]; //in the tipe of (48 + i)
	sleep(1);
	printf("\e[A\e[A             You are ");
	fflush(stdout);
	switch(my_number)
	{
		case '1':
			printf("\e[92mPlayer 1\e[39m\n");
			break;
		case '2':
			printf("\e[93mPlayer 2\e[39m\n");
			break;
		case '3':
			printf("\e[96mPlayer 3\e[39m\n");
			break;
		case '4':
			printf("\e[95mPlayer 4\e[39m\n");
			break;
	}
	printf("\r   \r\n");
	fflush(stdout);
	sleep(2);


	int players_quantity = (int)(recvline[1] - 48);
	sendline[1] = 'p'; //means playing
	printf("\r                        \r");
	fflush(stdout);

	for(i = 1; i < 5; i++)
	{
		switch(i)
		{
			case 1:
				printf("\e[92mPlayer  #1\e[39m | ");
				break;
			case 2:
				printf("\e[93mPlayer  #2\e[39m | ");
				break;
			case 3:
				printf("\e[96mPlayer  #3\e[39m | ");
				break;
			case 4:
				printf("\e[95mPlayer  #4\e[39m\n");
				break;
		}
	}
	
	printf("Stat:  OUT | Stat:  OUT | Stat:  OUT | Stat:  OUT\n");
	printf("Life:  100 | Life:  100 | Life:  100 | Life:  100\n");
	printf("\e[A\e[A\r");
	fflush(stdout);
	for(i = 1; i <= players_quantity; i++)
	{
		switch(i)
		{
			case 1:
				for(j = 0; j < 7; j++)
				{
					printf("\e[C");
				}
				printf(" \e[43mIN\e[49m");
				fflush(stdout);
				break;
			case 2:
				for(j = 0; j < 10; j++)
				{
					printf("\e[C");
				}
				printf(" \e[43mIN\e[49m");
				fflush(stdout);
				break;
			case 3:
				for(j = 0; j < 10; j++)
				{
					printf("\e[C");
				}
				printf(" \e[43mIN\e[49m");
				fflush(stdout);
				break;
			case 4:
				for(j = 0; j < 10; j++)
				{
					printf("\e[C");
				}
				printf(" \e[43mIN\e[49m");
				fflush(stdout);
				break;
		}
	}
	printf("\e[B\e[B\r\n");



	for(i = 0; i < MATRIX_WIDTH; i++)
	{
		printf("                  \n");
	}
	printf("\n");
	fflush(stdout);
	*pl_quantity = players_quantity;
	*pl_number = my_number;

}




void Send_movement_to_server(int sockfd, char* sendline, int* symbol_flag, int* my_number, struct sockaddr_in* servaddr, int pid)
{
	while(1)
	{
		fscanf(stdin, "%c", &sendline[0]);
		if((sendline[0] != '\n') && (*symbol_flag == 1) && (sendline[0] != '+'))
		{
			*symbol_flag = 0;
			sendline[1] = (char)(*my_number);
			sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) servaddr, sizeof(*servaddr));
		}	
		else if(sendline[0] == '\n')
		{
			printf("\e[A\r                        \r");
			fflush(stdout);
			*symbol_flag = 1;
		}
		else if((sendline[0] == '+'))
		{
			sendline[0] = '+';
			sendline[1] = (char)(*my_number);
			sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) servaddr, sizeof(*servaddr));
			printf("Goodbuy, see you later!\n");
			kill(pid, SIGKILL);
		}
	}
}


void Change_statistics(int sockfd, char* sendline, char* recvline, int* my_number, int* life, struct sockaddr_in* servaddr)
{
	int i;	
	if(recvline[MATRIX_SIZE + 1] == 'E')
	{
		printf("Congratulations, YOU are the winner!\n");
		kill(0, SIGKILL);
		//return 0;
	}
	if(recvline[MATRIX_SIZE] != '0')
	{
		for(i = 0; i < MATRIX_WIDTH + 3; i++)
		{
			printf("\e[A\r");
		}
		fflush(stdout);
		switch(recvline[MATRIX_SIZE] - 48)
		{
			case 1:
				for(i = 0; i < 7; i++)
				{
					printf("\e[C");
				}
				break;
			case 2:
				for(i = 0; i < 20; i++)
				{
					printf("\e[C");
				}
				break;
			case 3:
				for(i = 0; i < 33; i++)
				{
					printf("\e[C");
				}
				break;
			case 4:
				for(i = 0; i < 46; i++)
				{
					printf("\e[C");
				}
				break;
		}
		printf("\e[41mOUT\e[49m");
		for(i = 0; i < MATRIX_WIDTH + 3; i++)
		{
			printf("\e[B\r");
		}
		fflush(stdout);

	}
	if(recvline[MATRIX_SIZE + 2] != '0')
	{
		life[recvline[MATRIX_SIZE + 2] - 48 - 1] -= 20;
		for(i = 0; i < MATRIX_WIDTH + 2; i++)
		{
			printf("\e[A\r");
		}
		fflush(stdout);
		switch(recvline[MATRIX_SIZE + 2] - 48)
		{
			case 1:
				for(i = 0; i < 7; i++)
				{
					printf("\e[C");
				}
				break;
			case 2:
				for(i = 0; i < 20; i++)
				{
					printf("\e[C");
				}
				break;
			case 3:
				for(i = 0; i < 33; i++)
				{
					printf("\e[C");
				}
				break;
			case 4:
				for(i = 0; i < 46; i++)
				{
					printf("\e[C");
				}
				break;
		}
		printf("\e[41m%3d\e[49m", life[recvline[MATRIX_SIZE + 2] - 48 - 1]);
		if((life[*my_number - 48 - 1] == 0))
		{
			sendline[0] = '+';
			sendline[1] = recvline[MATRIX_SIZE + 2];
			sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) servaddr, sizeof(*servaddr));
			sendline[0] = 'p';
			for(i = 0; i < MATRIX_WIDTH + 2; i++)
			{
				printf("\e[B\r");
			}
			fflush(stdout);
			printf("NOO, you loooser!\n");
			kill(0, SIGKILL);
			//return 0;
		}
		for(i = 0; i < MATRIX_WIDTH + 2; i++)
		{
			printf("\e[B\r");
		}
		fflush(stdout);
	}

}


void Print_game_field(char* recvline)
{
	int i, j;
	for(i = 0; i < MATRIX_WIDTH; i++)
	{
		printf("\e[A");
		fflush(stdout);
	}
	for(i = 0; i < MATRIX_WIDTH; i++)
	{
		printf("            ");
		fflush(stdout);
		for(j = 0; j < MATRIX_LENGTH; j++)
		{
			if((recvline[MATRIX_LENGTH * i + j] < 49) || (recvline[MATRIX_LENGTH * i + j] > 57))
			{
				printf("%c", recvline[MATRIX_LENGTH * i + j]);
			}
			else if((recvline[MATRIX_LENGTH * i + j] > 53) && (recvline[MATRIX_LENGTH * i + j] < 58))
			{
				switch(recvline[MATRIX_LENGTH * i + j])
				{
					case '9':
						printf("\e[92m#\e[39m");
						break;
					case '8':
						printf("\e[93m#\e[39m");
						break;
					case '7':
						printf("\e[96m#\e[39m");
						break;
					case '6':
						printf("\e[95m#\e[39m");
						break;
				}
			}
			else
			{
				switch(recvline[MATRIX_LENGTH * i + j])
				{
					case '1':
						printf("\e[92m1\e[39m");
						break;
					case '2':
						printf("\e[93m2\e[39m");
						break;
					case '3':
						printf("\e[96m3\e[39m");
						break;
					case '4':
						printf("\e[95m4\e[39m");
						break;
				}
			}
				
			fflush(stdout);
		}
		printf("\n");
	}
}

