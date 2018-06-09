#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "semaphore.h"
#include <unistd.h>
#include <string.h>
#include <time.h>

/****array size********/
#define SIZE 30


/******array elements***********/
  struct buffer
  {
      char value;
      bool consARead;
      bool consBRead;
  };

/*****shared structure, cyclic list******/
  struct shared_memory
  {
        struct buffer list[SIZE];
        int head;
        int tail;
  };


/********method to put letter into array************/
void produce(struct shared_memory *X , char letter)
{
  X->list[X->tail].value = letter;
  X->list[X->tail].consARead = false;
  X->list[X->tail].consBRead = false;
  X->tail++;

  if(X->tail==SIZE)
    X->tail = (X->tail)%SIZE;
}

/*********method to remove letter from array**************/
char consume(struct shared_memory *X)
{
  char letter;
  letter = X->list[X->head].value;
  X->head++;
  if(X->head == SIZE)
    X->head = (X->head)%SIZE;

  return letter;
}

int main(int argc, char **argv)
{

  srand( time(NULL));

  /*********for shared memory******************/
  int shm_id;
  struct shared_memory *shared_mem_ptr, *prodA, *prodB,  *consA, *consB;

  /**********semaphores***********************/
  int mutex, empty, consA_full, consB_full;

  int i; //for random

  struct shmid_ds shmbuffer;
  const int shared_segment_size = sizeof(struct shared_memory) * SIZE;
  if(( shm_id = shmget(666, shared_segment_size, IPC_CREAT | 0600)) == -1 )
    perror("shmget");

  if(( shared_mem_ptr = (struct shared_memory *) shmat (shm_id, NULL, 0)) == (struct shared_memory *)-1 )
    perror("shmat");

    shared_mem_ptr->list[0].value = 'A';
    shared_mem_ptr->list[0].consARead = false;
    shared_mem_ptr->list[0].consBRead = false;
    shared_mem_ptr->list[1].value = 'B';
    shared_mem_ptr->list[1].consARead = false;
    shared_mem_ptr->list[1].consBRead = false;
    shared_mem_ptr->list[2].value = 'C';
    shared_mem_ptr->list[2].consARead = false;
    shared_mem_ptr->list[2].consBRead = false;
    shared_mem_ptr->head = 0;
    shared_mem_ptr->tail = 3;

  printf("List filled with A, B, C\n\n");
  mutex = Bsemcreate(".", 'X');
  empty = semcreate(".", 3214, SIZE-3);

  consA_full = semcreate(".", 'C', 0);
  consB_full = semcreate(".", 'D', 0);

  pid_t pid1, pid2, pid3;
  pid1 = fork();

  if(pid1 == 0)
  {
    pid2 = fork();

                  /********** Producer A   ***************/
    if(pid2 == 0)
    {
      prodA = shmat(shm_id, 0, 0);
      while(1)
      {
      // sleep(2);

        char randomletter = 'A' + (random() % 26);

        P(empty, 1);
        PB(mutex);

        produce(prodA, randomletter);
        if(prodA->tail != 0)
          printf("\nProducerA -> produce: %c\n", prodA->list[(prodA->tail)-1].value);
        else
          printf("\nProducerA -> produce: %c\n", prodA->list[(prodA->tail)].value);
        VB(mutex);
        V(consA_full, 1);
        V(consB_full, 1);
      }
    }

                   /********** Producer B   ***************/
    else
    {
      prodB = shmat(shm_id, 0, 0);
      while(1)
      {
       // sleep(2);

        P(empty, 2);
        PB(mutex);

        char randomletter = 'A' + (random() % 26);
        produce(prodB, randomletter);

        if(prodB->tail != 0)
          printf("\nProducerB -> produce: %c\n", prodB->list[(prodB->tail)-1].value);
        else
          printf("\nProducerB -> produce: %c\n", prodB->list[(prodB->tail)].value);

        randomletter = 'A' + (random() % 26);
        produce(prodB, randomletter);

        if(prodB->tail != 0)
          printf("ProducerB -> produce: %c\n", prodB->list[(prodB->tail)-1].value);
        else
          printf("ProducerB -> produce: %c\n", prodB->list[(prodB->tail)].value);

        VB(mutex);
        V(consA_full, 2);
        V(consB_full, 2);
      }
    }
  }


  else
  {
    pid3 = fork();

                /************** Consumer A *************/
    if(pid3 == 0)
    {
      consA = shmat(shm_id, 0, 0);
      while(1)
      {
      //  sleep(1);

            P(consA_full, 1);
            PB(mutex);
            if(consA->list[consA->head].consBRead)
            {
              printf("ConsumerA -> consumes: %c\n", consume(consA));
              V(empty, 1);
            }
            else if(!consA->list[consA->head].consARead)
            {
                consA->list[consA->head].consARead = true;
  
/*  Using i=rand to sign some elements as read, so the work was a bit faster and ConsumerA wasn't only the one to read elements and ConsumerB to remove them. Also to clean the place faster, so that ProducerB wasn't held for a time because of full list. */

                i = random()%SIZE;
                if(!consA->list[i].consARead)
                  consA->list[i].consARead = true;
            }
            else
            {
              i = random()%SIZE;
              if(!consA->list[i].consARead)
                consA->list[i].consARead = true;
            }
            VB(mutex);
      }
    }

                 /************** Consumer B *************/
    else
    {
      consB = shmat(shm_id, 0, 0);
      while(1)
      {
        //  sleep(1);

          P(consB_full, 1);
          PB(mutex);

          if(consB->list[consB->head].consARead)
          {
            printf("ConsumerB -> consumes: %c\n", consume(consB));
            V(empty, 1);
          }
          else if(!consB->list[consB->head].consBRead)
          {
              consB->list[consB->head].consBRead = true;
              i = random()%SIZE;

              if(!consB->list[i].consBRead)
                consB->list[i].consBRead = true;
          }
          else
          {
            i = random()%SIZE;
            if(!consB->list[i].consBRead)
              consB->list[i].consBRead = true;
          }
          VB(mutex);
      }
    }
  }
}
