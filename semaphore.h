#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

union semun{
    int val;            //value for SETVAL
    struct semid_ds *buf;   //buffer for IPC_STAT, IPC_SET
    unsigned short int *array;  //array for GETALL, SETALL
    struct seminfo *__buf;  //buffer for IPC_INFO
};

int Bsemcreate(char*, int);

int VB(int);

int PB(int);

int semvalue(int);

int Bdelete(int);

int semcreate(char*, int, int);

int P(int, int);
int V(int, int);

#endif
