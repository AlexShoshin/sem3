#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int msqid;
    char pathname[]="09-2a.c";
    key_t  key;
    int len, maxlen;
    int number1, number2;
    printf("Insert your numbers: ");
    scanf("%d %d", &number1, &number2);

    struct mymsgbuf
    {
        long mtype;
        int num1;
        int num2;
        int cpid;
    } mybuf;
    int i;
    key = ftok(pathname, 0);

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    int pid = getpid();

    mybuf.mtype = 1;
    mybuf.num1 = number1;
    mybuf.num2 = number2;
    mybuf.cpid = pid;
    len = sizeof(mybuf) - sizeof(long);

    if (msgsnd(msqid, &mybuf, len, 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    maxlen = sizeof(mybuf) - sizeof(long);
    if (( len = msgrcv(msqid, &mybuf, maxlen, pid, 0)) < 0)
    {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }
    printf("Your mult: %d", mybuf.num1);
    return 0;
}
