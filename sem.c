#include "semaphore.h"
#include <stdio.h>

//create binary semaphore
int Bsemcreate(char* path, int k)
{
key_t key;
int sem;
union semun set;
    if ((key = ftok(path, k)) == -1)
    return -1;
    if ((sem = semget(key, 1, IPC_CREAT | 0600)) == -1)
    return -1;
    set.val = 1;
    semctl(sem, 0, SETVAL, set);
    return sem;
}

//operation V
int VB(int s)
{
struct sembuf op;
int info;

    if ((info = semctl(s, 0, GETVAL)) == -1)
      return -1;
    if (info == 1)
      return 0;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    return semop(s, &op, 1);
}

//opration P
int PB(int s)
{
struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;

    return semop(s, &op, 1);
}

//read value
int semvalue(int s){

    return semctl(s, 0, GETVAL);
}

//number of processes waiting under P
int BPwait(int s){

    return semctl(s, 0, GETNCNT);
}

//delete a  semaphore
int Bdelete(int s){

    return semctl(s, 0, IPC_RMID);
}

//normal semaphores

int semcreate(char* path, int k, int value)
{
key_t key;
int sem;
union semun set;
    if ((key = ftok(path, k)) == -1)
      return -1;
    if ((sem = semget(key, 1, IPC_CREAT | 0600)) == -1)
      return -1;

    set.val = value;
    semctl(sem, 0, SETVAL, set);
      return sem;
}

int V(int s, int i)
{
struct sembuf op;
int info;

    if ((info = semctl(s, 0, GETVAL)) == -1)
      return -1;

    op.sem_num = 0;
    op.sem_op = i;
    op.sem_flg = 0;
    return semop(s, &op, 1);
}

//operation P
int P(int s, int i)
{
struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -i;
    op.sem_flg = 0;

    return semop(s, &op, 1);
}
