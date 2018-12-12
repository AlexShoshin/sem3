#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<malloc.h>

#define LINE_LEN 550
#define MATRIX_SIZE 216
#define FULL_MATRIX 220
#define MATRIX_WIDTH 12
#define MATRIX_LENGTH 18
#define MAX_PLAYERS 4
#define RECVLINE_LEN 3
#define SENDLINE_LEN 3
#define MAX_SPELLS 3


void Create_matrix(char * matrix);
void Create_coordinates(int* cur_x, int* cur_y, int* coord, int players_quantity);
int Scanf_pl_quantity();
void Execute_an_action(char* recvline, int* cur_x, int* cur_y, int player_ind, char* matrix, int player_face, int* ingame_flag, int* players_quantity, int* change_flag, int* spell_quantity);
void Move_spells(char* matrix, int* ingame_flag, int* place_flag, int* change_flag, int* side_flag, int* cur_x, int* cur_y, int* spell_quantity);




int main()
{
        int sockfd;
        int clilen, n;
	int line[LINE_LEN];
	int i, j, k;
	char recvline[RECVLINE_LEN];
	char sendline[SENDLINE_LEN];
        struct sockaddr_in servaddr, cliaddr;
	struct sockaddr_in clientaddr[MAX_PLAYERS];

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(51069);
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0))<0)
        {
                perror(NULL);
                exit(-1);
        }
        if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
        {
                perror(NULL);
                close(sockfd);
                exit(-1);
        }
		
	char matrix[FULL_MATRIX];
	Create_matrix(matrix);

	int players_quantity = Scanf_pl_quantity();

	int cur_x[MAX_PLAYERS];
	int cur_y[MAX_PLAYERS];
	int coord[MAX_PLAYERS];
	Create_coordinates(cur_x, cur_y, coord, players_quantity);

	for(i = 0; i < players_quantity; i++)
	{
		recvfrom(sockfd, recvline, RECVLINE_LEN, 0, (struct sockaddr *) &cliaddr, &clilen);
		if(recvline[1] == 'i')
		{
			printf("Player number %d connected\n(Connected: %d/%d)\n\n", (i + 1), (i + 1), players_quantity);
			sendline[0] = (char)(i + 49);
			sendline[1] = (char)(players_quantity + 48);
			sendline[2] = '\0';
			sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			matrix[coord[i]] = (char)(i + 49);	
			clientaddr[i] = cliaddr;
		}
	}
	int ingame_flag[MAX_PLAYERS];
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		ingame_flag[i] = 0;
	}
	sleep(4);
	for(i = 0; i < players_quantity; i++)
	{
		ingame_flag[i] = 1;
		sendto(sockfd, (char*)matrix, FULL_MATRIX, 0, (struct sockaddr *) &clientaddr[i], sizeof(clientaddr[i]));
	}
	printf("Server: starting the game\n");

	char player_face;
	int player_ind;
	int target;
	int side_flag = 1;
	int change_flag = 0;
	int place_flag[MATRIX_SIZE];
	for(i = 0; i < MATRIX_SIZE; i++)
	{
		place_flag[i] = 0;
	}
	int spell_quantity[MAX_PLAYERS];
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		spell_quantity[i] = 0;
	}
	while(players_quantity > 1)
	{
		recvfrom(sockfd, recvline, RECVLINE_LEN, 0, (struct sockaddr *) &cliaddr, &clilen);
		player_ind = (int)(recvline[1] - 48);
		player_face = recvline[1];
		Execute_an_action(recvline, cur_x, cur_y, player_ind, matrix, player_face, ingame_flag, &players_quantity, &change_flag, spell_quantity);

		Move_spells(matrix, ingame_flag, place_flag, &change_flag, &side_flag, cur_x, cur_y, spell_quantity);

		for(i = 0; i < MATRIX_SIZE; i++)
		{
			place_flag[i] = 0;
		}
		for(i = 0; i < MAX_PLAYERS; i++)
		{
			if(ingame_flag[i] == 1)
			{
				sendto(sockfd, (char*)matrix, FULL_MATRIX, 0, (struct sockaddr *) &clientaddr[i], sizeof(clientaddr[i]));
			}
		}
		printf("SENT\n");
		matrix[MATRIX_SIZE] = '0';
		matrix[MATRIX_SIZE + 1] = '0';
		matrix[MATRIX_SIZE + 2] = '0';
		matrix[MATRIX_SIZE + 3] = '0';

	}


	for(i = 0; i < MAX_PLAYERS; i++)
	{
		if(ingame_flag[i] == 1)
		{
			matrix[MATRIX_SIZE] = i + 1;
			matrix[MATRIX_SIZE + 1] = 'E';
			sendto(sockfd, (char*)matrix, FULL_MATRIX, 0, (struct sockaddr *) &clientaddr[i], sizeof(clientaddr[i]));
		}
	}

	close(sockfd);
	printf("Server: finish\n");
	return 0;
}






void Create_matrix(char * matrix)
{
	int i, j, k;
	for(i = 0; i < MATRIX_SIZE; i++)
	{
		matrix[i] = ' ';
	}
	for(i = 1; i < MATRIX_LENGTH; i++)
	{
		matrix[i] = '-';
		matrix[(MATRIX_WIDTH - 1) * MATRIX_LENGTH + i] = '-';
	}
	for(i = 1; i < MATRIX_WIDTH; i++)
	{
		matrix[MATRIX_LENGTH * i - 1] = '|';
		matrix[MATRIX_LENGTH * i] = '|';
	}
	matrix[0] = '0';
	matrix[MATRIX_LENGTH - 1] = '0';
	matrix[MATRIX_SIZE - MATRIX_LENGTH] = '0';
	matrix[MATRIX_SIZE - 1] = '0';

	matrix[MATRIX_SIZE] = '0';
	matrix[MATRIX_SIZE + 1] = '0';
	matrix[MATRIX_SIZE + 2] = '0';
	matrix[MATRIX_SIZE + 3] = '0';
	matrix[MATRIX_SIZE + 4] = '\0';
}

void Create_coordinates(int* cur_x, int* cur_y, int* coord, int players_quantity)
{
	int i;
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		switch(i)
		{
			case 0:
				cur_x[i] = 1;
				cur_y[i] = 1;
				break;
			case 1:
				cur_x[i] = MATRIX_LENGTH - 2;
				cur_y[i] = MATRIX_WIDTH - 2;
				break;
			case 2:
				cur_x[i] = 1;
				cur_y[i] = MATRIX_WIDTH - 2;
				break;
			case 3:
				cur_x[i] = MATRIX_LENGTH - 2;
				cur_y[i] = 1;
				break;
		}
	}

	for(i = 0; i < players_quantity; i++)
	{
		switch(i)
		{
			case 0:
				coord[i] = MATRIX_LENGTH + 1;
				break;
			case 1:
				coord[i] = MATRIX_SIZE - MATRIX_LENGTH - 2;
				break;
			case 2:
				coord[i] = MATRIX_SIZE - MATRIX_LENGTH * 2 + 1;
				break;
			case 3:
				coord[i] = (MATRIX_LENGTH - 1) * 2;
				break;
		}
	}
}

int Scanf_pl_quantity()
{
	int players_quantity;
	char pl_choice;
	printf("Server: start.\nHow many players shall be?(2,3,4)\n");
	scanf("%c", &pl_choice);
	while((pl_choice != '2') && (pl_choice != '3') && (pl_choice != '4'))
	{
		while(pl_choice != '\n')
		{
			scanf("%c", &pl_choice);
		}
		printf("\r\e[A                       \r");
		scanf("%c", &pl_choice);
	}
	players_quantity = (int)(pl_choice - 48);
	printf("Waiting for players...(Connected: 0/%d)\n", players_quantity);
	return players_quantity;

}

void Execute_an_action(char* recvline, int* cur_x, int* cur_y, int player_ind, char* matrix, int player_face, int* ingame_flag, int* players_quantity, int* change_flag, int* spell_quantity)
{
	int target;
	int k;
	printf("Received %c from player № %d\n", recvline[0], player_ind);
	switch(recvline[0])
	{
		case 'w':
			if((cur_y[player_ind - 1] != 1) && (matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] - 1) + cur_x[player_ind - 1]]  == ' '))
			{
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]]  = ' ';
				cur_y[player_ind - 1]  -= 1;
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = player_face;
			}
			break;
		case 'a':
			if((cur_x[player_ind - 1] != 1) && (matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1] - 1]  == ' '))
			{	
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = ' ';
				cur_x[player_ind - 1]  -= 1;
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = player_face;
			}
			break;
		case 's':
			if((cur_y[player_ind - 1] != MATRIX_WIDTH - 2) && (matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] + 1) + cur_x[player_ind - 1]]  == ' '))
			{	
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = ' ';
				cur_y[player_ind - 1]  += 1;
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = player_face;
			}
			break;
		case 'd':
			if((cur_x[player_ind - 1] != MATRIX_LENGTH - 2) && (matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1] + 1]  == ' '))
			{	
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = ' ';
				cur_x[player_ind - 1]  += 1;
				matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = player_face;
			}
			break;
		case '+':
			matrix[MATRIX_LENGTH * cur_y[player_ind - 1] + cur_x[player_ind - 1]] = ' ';
			matrix[MATRIX_SIZE] = (char)(player_ind + 48);
			matrix[MATRIX_SIZE + 1] = 'O';
			printf("Player %d finished the game\n", player_ind);
			*players_quantity -= 1;
			ingame_flag[player_ind - 1] = 0;
			printf("Players in game: %d\n", *players_quantity);
			break;
		case ' ':
			for(k = 0; k < *players_quantity; k++)
			{
				if((ingame_flag[k] == 1) && (k != (player_ind - 1)))
				{
					target = k;
				}
			}
			if(spell_quantity[target] < MAX_SPELLS)
			{
				if((cur_x[target] > cur_x[player_ind - 1]) && (cur_y[target] < cur_y[player_ind - 1]))
				{
					matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] - 1) + cur_x[player_ind - 1] + 1] = (char)(10 - (target + 1) + 48);
					*change_flag = 1;
				}
				else if((cur_x[target] > cur_x[player_ind - 1]) && (cur_y[target] >= cur_y[player_ind - 1]))
				{
					matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] + 1) + cur_x[player_ind - 1] + 1] = (char)(10 - (target + 1) + 48);
					*change_flag = 1;
				}
				else if((cur_x[target] <= cur_x[player_ind - 1]) && (cur_y[target] < cur_y[player_ind - 1]))
				{
					matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] - 1) + cur_x[player_ind - 1] - 1] = (char)(10 - (target + 1) + 48);
					*change_flag = 1;
				}
				else if((cur_x[target] <= cur_x[player_ind - 1]) && (cur_y[target] >= cur_y[player_ind - 1]))
				{
					matrix[MATRIX_LENGTH * (cur_y[player_ind - 1] + 1) + cur_x[player_ind - 1] - 1] = (char)(10 - (target + 1) + 48);
					*change_flag = 1;
				}

				spell_quantity[target] += 1;
			}
			break;
	}
}


void Move_spells(char* matrix, int* ingame_flag, int* place_flag, int* change_flag, int* side_flag, int* cur_x, int* cur_y, int* spell_quantity)
{
	int i, j;
	for(i = 0; i < MATRIX_WIDTH; i++)
	{
		for(j = 0 ; j < MATRIX_LENGTH; j++)
		{
			if((matrix[MATRIX_LENGTH * i + j] > 53) && (matrix[MATRIX_LENGTH * i + j] < 58) && (place_flag[MATRIX_LENGTH * i + j] != 1))
			{
				if((cur_x[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] == j) && (cur_y[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] == i))
				{
					matrix[MATRIX_LENGTH * i + j] = ' ';
				}
				else
				{
				while(*change_flag == 0)
				{
					if(*side_flag == 0)
					{
						if(cur_x[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] < j)
						{
							if((matrix[MATRIX_LENGTH * i + j - 1] < 49) || (matrix[MATRIX_LENGTH * i + j - 1] > 57))
							{
								matrix[MATRIX_LENGTH * i + j - 1] = matrix[MATRIX_LENGTH * i + j];
								place_flag[MATRIX_LENGTH * i + j - 1] = 1;
							}
							else if((matrix[MATRIX_LENGTH * i + j - 1] > 53) && (matrix[MATRIX_LENGTH * i + j - 1] < 58))
							{
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j - 1] - 48)] -= 1;
								matrix[MATRIX_LENGTH * i + j - 1] = ' ';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							else
							{
								matrix[MATRIX_SIZE + 2] = matrix[MATRIX_LENGTH * i + j - 1];
								matrix[MATRIX_SIZE + 3] = 'C';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							matrix[MATRIX_LENGTH * i + j] = ' ';
							*change_flag = 2;
						}
						if(cur_x[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] > j)
						{
							if((matrix[MATRIX_LENGTH * i + j + 1] < 49) || (matrix[MATRIX_LENGTH * i + j + 1] > 57))
							{
								matrix[MATRIX_LENGTH * i + j + 1] = matrix[MATRIX_LENGTH * i + j];
								place_flag[MATRIX_LENGTH * i + j + 1] = 1;
							}
							else if((matrix[MATRIX_LENGTH * i + j + 1] > 53) && (matrix[MATRIX_LENGTH * i + j + 1] < 58))
							{
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j + 1] - 48)] -= 1;
								matrix[MATRIX_LENGTH * i + j + 1] = ' ';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							else
							{
								matrix[MATRIX_SIZE + 2] = matrix[MATRIX_LENGTH * i + j + 1];
								matrix[MATRIX_SIZE + 3] = 'C';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							matrix[MATRIX_LENGTH * i + j] = ' ';
							*change_flag = 2;
						}
						if(cur_x[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] == j)
						{
							*side_flag = 1;
						}
					}
					if(*side_flag == 1)
					{
						if(cur_y[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] < i)
						{
							if((matrix[MATRIX_LENGTH * (i - 1) + j] < 49) || (matrix[MATRIX_LENGTH * (i - 1) + j] > 57))
							{
								matrix[MATRIX_LENGTH * (i - 1) + j] = matrix[MATRIX_LENGTH * i + j];
								place_flag[MATRIX_LENGTH * (i - 1) + j] = 1;
							}
							else if((matrix[MATRIX_LENGTH * (i - 1) + j] > 53) && (matrix[MATRIX_LENGTH * (i - 1) + j] <  58))
							{
								spell_quantity[9 - (matrix[MATRIX_LENGTH * (i - 1) + j] - 48)] -= 1;
								matrix[MATRIX_LENGTH * (i - 1) + j] = ' ';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							else
							{
								matrix[MATRIX_SIZE + 2] = matrix[MATRIX_LENGTH * (i - 1) + j];
								matrix[MATRIX_SIZE + 3] = 'C';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							matrix[MATRIX_LENGTH * i + j] = ' ';
							*change_flag = 1;
						}
						if(cur_y[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] > i)
						{
							if((matrix[MATRIX_LENGTH * (i + 1) + j] < 49) || (matrix[MATRIX_LENGTH * (i + 1) + j] > 57))
							{
								matrix[MATRIX_LENGTH * (i + 1) + j] = matrix[MATRIX_LENGTH * i + j];
								place_flag[MATRIX_LENGTH * (i + 1) + j] = 1;
							}
							else if((matrix[MATRIX_LENGTH * (i + 1) + j] > 53) && (matrix[MATRIX_LENGTH * (i + 1) + j] < 58))
							{
								spell_quantity[9 - (matrix[MATRIX_LENGTH * (i + 1) + j] - 48)] -= 1;
								matrix[MATRIX_LENGTH * (i + 1) + j] = ' ';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							else
							{
								matrix[MATRIX_SIZE + 2] = matrix[MATRIX_LENGTH * (i + 1) + j];
								matrix[MATRIX_SIZE + 3] = 'C';
								spell_quantity[9 - (matrix[MATRIX_LENGTH * i + j] - 48)] -= 1;
							}
							matrix[MATRIX_LENGTH * i + j] = ' ';
							*change_flag = 1;
						}
						if(cur_y[10 - (int)(matrix[MATRIX_LENGTH * i + j] - 48) - 1] == i)
						{
								*side_flag = 0;
						}
					}
				}
				*side_flag = *change_flag - 1;
				*change_flag = 0;
				}
			}
		}
	}
}




