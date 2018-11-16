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

//Attention!This is the wiper and it should be launched first,
//because is creates semafors and msg queue(во избежание путаницы)

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
        int wiping_times[DISHES_TYPES_QUANTITY];
        for(i = 0; i < DISHES_TYPES_QUANTITY; i++)
        {
                fscanf(dishes_times, "%s", buf_string);
                if(strcmp(buf_string, "plate") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        fscanf(dishes_times, "%d", &time);
			wiping_times[0] = time;
                }
                else if(strcmp(buf_string, "spoon") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        fscanf(dishes_times, "%d", &time);
			wiping_times[1] = time;
                }
		else if(strcmp(buf_string, "cup") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        fscanf(dishes_times, "%d", &time);
			wiping_times[2] = time;
                }
                else if(strcmp(buf_string, "knife") == 0)
                {
                        fscanf(dishes_times, "%d", &time);
                        fscanf(dishes_times, "%d", &time);
			wiping_times[3] = time;
                }
        }
	fclose(dishes_times);
        int msgid, semid;
	int len;
        key_t key;
        char pathname[] = "dishes_msg.c";
        key = ftok(pathname, 0);
        if((msgid = msgget(key, 0)) > 0)
	{
		msgctl(msgid, IPC_RMID, 0);
	}
	msgid = msgget(key, 0666 | IPC_CREAT);

        if((semid = semget(key, 1, 0)) > 0)
        {
                semctl(semid, IPC_RMID, 0);
        }
        semid = semget(key, 1, 0666 | IPC_CREAT);

        int sleeping_time;
	len = sizeof(char);
	printf("Wiper: WAITING then something appears on the table!\n\n");

	while(msgrcv(msgid, &mybuf, len, 1, 0))
	{
		execsem(0, 1, semid);
		switch(mybuf.dish_type)
		{
			case 'p':
				buf_string = "plate";
				break;
			case 's':
				buf_string = "spoon";
				break;
			case 'c':
				buf_string = "cup";
				break;
			case 'k':
				buf_string = "knife";
				break;
			case ENDING_MSG_ID:
				printf("Wiper: and i'm free too!\n");
				return 0;
		}
		printf("Wiper: STARTED wiping the %s\n\n", buf_string);

        	if(strcmp(buf_string, "plate") == 0)
        	{
                	sleeping_time = wiping_times[0];
                	sleep(sleeping_time);
        	}
        	else if(strcmp(buf_string, "spoon") == 0)
        	{
                	sleeping_time = wiping_times[1];
                	sleep(sleeping_time);
        	}
        	else if(strcmp(buf_string, "cup") == 0)
                {
                        sleeping_time = wiping_times[2];
                        sleep(sleeping_time);
                }
                else if(strcmp(buf_string, "knife") == 0)
                {
                        sleeping_time = wiping_times[3];
                        sleep(sleeping_time);
                }
		printf("Wiper: ENDED wiping the %s\n\n", buf_string);
		printf("Wiper: WAITING then something appears on the table!\n");
        }
	
	free(buf_string);
        return 0;
}

