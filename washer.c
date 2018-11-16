#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>

#define TABLE_LIMIT 2
#define DISHES_TYPES_QUANTITY 4
#define ENDING_MSG_ID 66

//Attention! this is washer, and it should be launched after the wiper!

void execsem(int number, int digit, int semid)
{
	struct sembuf mysembuf;
        mysembuf.sem_num = number;
        mysembuf.sem_op = digit;
        mysembuf.sem_flg = 0;
        semop(semid, &mysembuf, 1);
}

int main()
{
	int i;
	int time;
	struct mymsgbuf
	{
		long mtype;
		char dish_type;
	}mybuf;

	char* buf_string = (char*)malloc(50 * sizeof(char));
	FILE* dishes_times = fopen("dishes_times.txt", "r");
	int washing_times[DISHES_TYPES_QUANTITY]; 
	for(i = 0; i < DISHES_TYPES_QUANTITY; i++)
	{
		fscanf(dishes_times, "%s", buf_string);
		if(strcmp(buf_string, "plate") == 0)
		{
			fscanf(dishes_times, "%d", &time);
			washing_times[0] = time;
			fscanf(dishes_times, "%d", &time);
		}
		else if(strcmp(buf_string, "spoon") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        washing_times[1] = time;
			fscanf(dishes_times, "%d", &time);
                } 
		else if(strcmp(buf_string, "cup") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        washing_times[2] = time;
			fscanf(dishes_times, "%d", &time);
                } 
		else if(strcmp(buf_string, "knife") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        washing_times[3] = time;
			fscanf(dishes_times, "%d", &time);
                } 
	}

	int msgid, semid;
	int len;
	key_t key;
	char pathname[] = "dishes_msg.c";
	key = ftok(pathname, 0);
	msgid = msgget(key, 0);

        semid = semget(key, 1, 0);
        execsem(0, TABLE_LIMIT, semid);

	FILE* dishes_heap = fopen("dishes_heap.txt", "r");
	int dishes_quantity;
	int sleeping_time;
	fscanf(dishes_heap, "%d", &dishes_quantity);
	for(i = 0; i < dishes_quantity; i++)
	{
		fscanf(dishes_heap, "%s", buf_string);
		printf("Washer: STARTED to wash the %s\n\n", buf_string);
		if(strcmp(buf_string, "plate") == 0)
                {
			sleeping_time = washing_times[0];
			sleep(sleeping_time);
                }
                else if(strcmp(buf_string, "spoon") == 0)
                {
			sleeping_time = washing_times[1];
                        sleep(sleeping_time);
                }
                else if(strcmp(buf_string, "cup") == 0)
                {
			sleeping_time = washing_times[2];
                        sleep(sleeping_time);
                }
                else if(strcmp(buf_string, "knife") == 0)
                {
			sleeping_time = washing_times[3];
                        sleep(sleeping_time);
                }
		
		printf("Washer: WAITING then the table will have free place\n");
		execsem(0, -1, semid);
		mybuf.mtype = 1;
		mybuf.dish_type = buf_string[0];
		len = sizeof(char);
		msgsnd(msgid, &mybuf, len, 0);
		printf("Washer: PUT the %s on the table\n\n", buf_string);
	}
	printf("Washer: what's all, now i'm free!\n");
	mybuf.mtype = 1;
	mybuf.dish_type = ENDING_MSG_ID;
	len = sizeof(char);
        msgsnd(msgid, &mybuf, len, 0);

	free(buf_string);
	fclose(dishes_times);
	fclose(dishes_heap);
	return 0;
}

