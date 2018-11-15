#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void execsem(int number, int digit, int semid)
{
    struct sembuf mysembuf;

    mysembuf.sem_num = number;
    mysembuf.sem_op = digit;
    mysembuf.sem_flg = 0;
    semop(semid, &mysembuf, 1);
}


int main(void)
{
    int maxproc = 2;
    int msqid;
    char pathname[]="09-2a.c";
    key_t  key;
    int len, maxlen;

    struct mymsgbuf
    {
        long mtype;
        int num1;
        int num2;
        int cpid;
    } mybuf;

    int i;
    int pid;
    key = ftok(pathname, 0);
    
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    int mult;

    int semid;
    char sempathname[]="08semaf.c";
    key_t semkey;
    semkey = ftok(sempathname, 0);

    if((semid = semget(semkey, 1, 0)) > 0)
    {
        semctl(semid, IPC_RMID, 0);  
    }
    semid = semget(semkey, 1, 0666 | IPC_CREAT);
    execsem(0, maxproc, semid);


    while(1)
    {
        maxlen = sizeof(mybuf) - sizeof(long);
	
        if (( len = msgrcv(msqid, &mybuf, maxlen, 1, 0)) < 0)
	{
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
	execsem(0, -1, semid);

	pid = fork();
	if(pid == 0)
	{
            mult = mybuf.num1 * mybuf.num2;
	    sleep(10);
            mybuf.mtype = mybuf.cpid;
            mybuf.num1 = mult;
       
            len = sizeof(mybuf) - sizeof(long);
            if (msgsnd(msqid, &mybuf, len, 0) < 0)
    	    {
                printf("Can\'t send message to queue\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                exit(-1);
            }
	    execsem(0, 1, semid);
	}
    }

    return 0;
}
